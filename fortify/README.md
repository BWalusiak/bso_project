# Fortify Source
## Opis
Makro FORTIFY_SOURCE zapewnia obsługę wykrywania BOF w różnych funkcjach libc, które wykonują operacje na pamięci i stringach. Nie wszystkie typy BOF są wykrywane, ale zapewnia ono dodatkowy poziom walidacji dla funkcji, które mogą być źródłem błędów.

Do działania Fortify niezbędna jest flaga optymalizacji `-O1` lub wyższa.

Fortify dzieli się na 2 poziomy:

 - -D_FORTIFY_SOURCE=1 - wykonuje sprawdzenie które nie powinny zakłócić działania programu.
 - -D_FORTIFY_SOURCE=2 - wykonuje sprawdzenie które mogą zakłócić działania programu, zapobiega exploitom string format i zmienia interpretacje stuktur przy ocenie BOF.


```c
struct S {
  struct T {
    char buf[5];
    int x;
  }
  t;
  char buf[20];
}
var;
```

W przypadku poziomu 1 `strcpy (&var.t.buf[1], "abcdefg");` nie jest uznawane za BOF.

W przypadku poziomu 2 `strcpy (&var.t.buf[1], "abcdefg");` jest uznawane za BOF.

Fortify Source nie ma wpływu na performance aplikacji ale może nieznacznie wpłynąć na rozmiar binarki.

## Clang i GCC

Fortify nie jest domyślnie używane przez kompilatory. Aby je włączyć używamy flagi `-D_FORTIFY_SOURCE={1,2}` oraz flagi optymalizacji `-O1` lub wyższej.

## Ochrona

### 1 - podmiana funkcji

Przy użwywaniu niebezpiecznych funkcji (memcpy, mempcpy, memmove, memset, strcpy, stpcpy, strncpy, strcat, strncat, sprintf, vsprintf, snprintf, vsnprintf, gets), zostaną one zastąpione przez ich bezpieczne odpowiedniki.

![](https://i.imgur.com/mSdz8tz.png)

W przypadku BOF program zostanie zakończony.

![](https://i.imgur.com/mBgRgEX.png)

### 2 - structy
Do demonstracji wykorzystam przykładowy struct:
```c
struct S {
  struct T {
    char buf[5];
    int x;
  }
  t;
  char buf[20];
}
var;
```

W przypadku testowego structa przy poziomie 1 nie jest wykrywany BOF ponieważ nie przekraczamy wielkości całej struktury.
```c
strcpy (&var.t.buf[1], "abcdefg");
```

![](https://i.imgur.com/v1zYEDt.png)

W przypadku testowego structa przy poziomie 1 BOF kończy program ponieważ ponieważ przekraczamy wielkości bufforu do którego zapisujemy.

![](https://i.imgur.com/QJQ2BDG.png)

### 3 - format string

Fortify Source poziom 2 zapobiega też exploitowi format string.

W przypadku próby odczytu pozycji ze stosu działanie programu jest przerywane.

![](https://i.imgur.com/R5DEjss.png)

Próba nadpisu wartości na stosie również zakańcza działanie programu.

![](https://i.imgur.com/yQ1xLJF.png)

## Wnioski
Dobrze jest używać fortify source przy kompilacji właściwie każdego oprogramowania ponieważ pozwala na znalezienie potencjalnych błędów podczas kompilacji. W przypadku używania poziomu 2 niezbędne są testy sprawdzające czy nie wprowadzone zostało nieprzewidziane zachowanie.
