# CET
## Opis
`Intel CET` to technika zapobiegająca atakom typu `ROP/JOP/COP` zaimplementowana w procesorach Intela od wersji Tiger Lake (11) oraz vPRO. Można podzielić ją na dwie osobne części.
### Indirect Branch Tracking

IBT dodaje do architektury x86 nową instrukcję - `endbranch`. Oznacza ona poprawny cel instrukcji call lub jump. Zaimplementowana w procesorze maszyna stanów zapewnia zakończenie działania programu w przypadku braku `endbranch` po skoku. W przypadku procesorów bez implementacji CET instrukcja `endbranch` traktowana jest jak `NOP`. Implementacja sprzętowa nie ma wpływu na performance. Podobny feature implementuje architektura ARM64 pod nazwą [BTI](https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/BTI--Branch-Target-Identification-).

![](https://i.imgur.com/umnGCCx.png)

Software'ową implementacje Forward-Edge CFI posiada clang pod nazwą [clang-cfi](https://clang.llvm.org/docs/ControlFlowIntegrity.html#forward-edge-cfi-for-virtual-calls) oraz Windows pod nazwą [Control Flow Guard](https://docs.microsoft.com/en-us/windows/win32/secbp/control-flow-guard).

Mają one znikomy wpływ na performance małych binarek (clang-cfi około 1%) ale obydwie implementacje odnowowały problemy w przypadku dużych aplikacji np. chromium.

### Shadow Stack

Shadow Stack to drugi stos, całkowicie oddzielny od zwykłego stosu przechowywujący tylko i wyłącznie adresy powrotu `RET`. Jego modyfikacja możliwa jest jedynie poprzez osobny zestaw instrukcji co uniemożliwia jego prostą edycje. Przy każdym wykonaniu instrukji ret porównywany jest adres powrotu zwykłego stosu i stosu z shadow stackiem i w przypadku rozbieżności działanie programu jest zakańczane. 

![](https://i.imgur.com/Zs2HCpl.png)

Shadow Stack jest implementowany również przez AMD począwszy od procesorów z mobilnej serii Ryzen 5000. Dla architektury x86 nie ma obecnie software'owego odpowiednika.

Na architekturze ARM64 istnieje software'owa implementacja shadow stacka. Jest ona zaimplementowana w [Androidzie](https://source.android.com/devices/tech/debug/shadow-call-stack). 


## Exploit
Nie posiadam mobilnego procesora z serii Tiger Lake więc do przetestowania metody CET spróbowałem wykorzystać emulator dostarczony przez intela - [Intel SDE](https://software.intel.com/content/www/us/en/develop/articles/intel-software-development-emulator.html).

Niestety zastosowane zgodnie z [instrukcją](https://software.intel.com/content/www/us/en/develop/articles/emulating-applications-with-intel-sde-and-control-flow-enforcement-technology.html) narzędzie odmówiło współpracy w przypadku mojej aplikacji.

![](https://i.imgur.com/KKmUZwp.png)

Nie może być to problem z maszyną wirtualną ani z systemem kali linux ponieważ emulator poprawnie wykonuje program `/bin/sh`:

![](https://i.imgur.com/7EfM7i8.png)

Ponieważ dokumentacja emulatora nie istnieje, nie byłem w stanie przetestować exploita rop (zaprezentowanego przy opisie ASLR).
