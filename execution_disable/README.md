# Execution Disable
## Opis
`Execution Disable`, zwane też `Executable space protection`, `NX` lub `W^X` oznacza obszary pamięci jako `non-executable`. Wykonywanie tak oznaczonego kodu spowoduje SEGFAULT oraz zabicie procesu.

Najczęstszą implementacją `Execution Disable` jest `NX bit` - sprzętowa implementacja stosowana w większości procesorów (x86 - intel pod nazwą `XD`, amd pod nazwą `NX bit` oraz ARM - pod nazwą `execute never`).

Możliwa jest też software'owa emulacja (nie na Windowsie) tej funkcjonalności ale wiąże się to ze znacznym spadkiem wydajności.
Niektóre dystrybucje linuxa x86-32bit mają domyślnie wyłączony NX bit (ubuntu, fedora).

NX zapobiega bezpośredniemu wykonywaniu wstrzykniętego na stos kodu. Pomimo tego w przypadku zastosowania niebezpiecznych funkcji nie powstrzymuje nas on przed atakami jedynie sterującymi działaniem programu, np `ret2libc`.

## Clang i GCC

W obydu kompilatorach NX jest defaultowo włączony. W GCC możemy wyłączyć nx przez flagę `-z execstack` a w Clang'u przez flagę `-fno-sanitize=safe-stack`.

