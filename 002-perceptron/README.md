# 002-perceptron

Jediný neuron se dvěma vstupy porovnává sigmoidální a ReLU aktivaci
na pravdivostní tabulce AND. Runner provádí tři běhy s MSE, BGD,
10000 epochami a rychlostí učení 0,2: sigmoida se seedem 42,
ReLU se stejným seedem a ReLU s pevným počátečním nastavením.
Po každém běhu vypíše spojité výstupy na trénovací tabulce.
[Architektura, metodika a výsledky](spec/specifikace-perceptron.md)
jsou v české specifikaci.

## Překlad, testy a spuštění

Linux x86-64:

```sh
make PLATFORM=linux_x86_64 test
make PLATFORM=linux_x86_64 exec
```

Windows x86-64 s MSYS2 MinGW-w64:

```sh
mingw32-make PLATFORM=win32_x86_64 test
mingw32-make PLATFORM=win32_x86_64 exec
```

Výstupy platforem jsou oddělené v `.cache/<platform>/`.
Testovací program používá sdílenou knihovnu `lib/unit`.

## Dokumentace zdrojového kódu

Veřejné rozhraní obsahuje anglické komentáře pro Doxygen.
Soubor `Doxyfile` ukládá případný výstup do `.cache/doxygen/`.
Dokumentace se při překladu ani testech nevytváří.
