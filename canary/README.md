# Canary
## Opis
`Stack Canary` jest metodą zabezpieczania stosu. Polega ona na umieszczeniu na stosie przed adresem powrotu losowo generowanej wartości. Przed wykonaniem instrukcji `ret` sprawdzana jest wartość kanarka. Jeżeli została zmodyfikowana działanie programu jest zatrzymywane z błęden `stack smashing detected`.

Wartość kanarka z reguły kończy się bajtami `\x00` - znakiem końca stringa. Ma to na celu utrudnienie wycieku.

Kanarek może być generowany na różne sposoby. Wyróżnia się 3:

 - `Terminator canaries` - kanarki składające się ze znaków typu `\x00`, `CR`, `LF`, `\xFF`. Ma on na celu utrudnienie nadpisania atakującemu poprzez wykorzystanie `terminator chars` - znaków zakańczających działanie funkcji które mogły by być użyte w exploicie.
 - `Random canaries` - wartość kanarka jest całkowicie losowa (generowana za pomocą `entropy-gathering daemon`). Aby atakujący nie mógł poznać wartośći kanarka zmienna globalna w której przechowywany jest porównawczy kanarek jest oddzielona kilkoma niemapowanymi stronami od reszty pamięci.
 - `Random XOR canaries` - Jest to `Random Canary` z tym że jego wartości zostały wymieszane operacją `XOR` z wartościami kontrolnymi zawartymi w programie. Jest on trudniejszy do złamania niż zwykły `Random Canary` ponieważ atakujący musi poznać jeszcze wartości kontrolne oraz algorytm mieszania.

Sprawdzenie wartości kanarka wygląda następująco:

![](https://i.imgur.com/1Nc7LFg.png)

Kanarek ładowany jest do rejestru eax, następnie odejmowana jest wartość porównawcza `gs:0x14`. Jeśli wartości były równe w rejestrze `eax` zostanie wartość 0 i skok `je` ominie `call` zakańczający program. Potem nastąpi zwykły powrót z funkcji.

## Clang i GCC

W obydwu kompilatorach kanarek jest defaultowo wyłączony ze względu na wydajność (na niektórych dystrybucjach linuxa np. arch, OpenBSD kompilatory domyślnie używają flagi `fstack-protector`). Podobnie zachowuje się kompilator intela - [link](https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/compiler-options/compiler-option-details/data-options/fstack-protector.html).

W gcc możliwe opcje kanarka to:

 - `fstack-protector` - kompilator używa kanarka dla funkcji używającyh `alloca()` lub z bufforami > 8
 - `-fstack-protector-all` - kompilator używa kanarka dla wszystkich funkcji
 - `-fstack-protector-strong` - `fstack-protector` + funkcje z lokacnymi tablicami oraz odniesieniami do ramki stosu (liczą się tylko zmienne faktycznie trafiające na stos, kompilator nie bierze pod uwagę tych wyoptymalizowanych lub przechowywanych w rejestrach)
 - `-fstack-protector-explicit` - kompilator używa tylko kanarka dla funkcji z atrybutem `stack_protect`

[Dokumentacja gcc](https://gcc.gnu.org/onlinedocs/gcc-11.1.0/gcc/Instrumentation-Options.html#Instrumentation-Options) nie specyfikuje domyślnego zachowania, ale z przeprowadzonego przeze testu wynika że kanarek jest domyślnie wyłączony.

### Test
Kompilowana aplikacja:

```{c}
#include <stdio.h>
#include <stdlib.h>

void func() {
  char input[512];
  char *buff=alloca(10);

  printf("Please enter your name: \n");
  fgets(input,512,stdin);
  fgets(buff,10,stdin);
  printf("Hi %s\n", input);
  printf(buff);
}

int main() {
  func();
  return 0;
}
```

Sprawdzenie za pomocą narzędzia checksec:

![](https://i.imgur.com/ukniQRh.png)

Moja wersja gcc: `gcc (Debian 10.2.1-6) 10.2.1 20210110`
