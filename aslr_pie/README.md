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

ASLR najlepiej stosować razem z PIE. W przeciwnym wypadku kod naszej aplikacji jest ładowany statycznie. Z tego powodu ma znaczący wpływ na wydajność na Linuxie. Jest on też opcją kontrolowaną flagą systemową dla wszystkich programów.

Na windowsie ASLR nie ma wpływu na wydajność. Opcja `/DYNAMICBASE` modyfikuje nagłówek executable aby wskazać czy aplikacja powinna być rebase'owana przy ładowaniu.

## Clang i GCC
### ASLR
Jest niezależny od kompilacji - można go włączyć lub wyłączyć w każdym momencie zmieniając flagę systemową komendami: 

Wyłączenie aslr: `echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`
Włączenie aslr: `echo 2 | sudo tee /proc/sys/kernel/randomize_va_space`

### PIE

W obydu kompilatorach PIE jest defaultowo włączone. W obydu kompilatorach możemy wyłączyć je przez flagę `-no-pie`.

Wartym zauważenia wyjątkiem jest kompilacja statyczna z włączonym PIE. Wykorzystując flagę `-static-pie` możemy utwożyć pie executable nie wymagające dynamicznego linkera. Niezbędna do tego jest kompilacja wszystkich linkowanych plików z flagą `-fpie`.

## 
