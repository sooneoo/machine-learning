# 002-perceptron

Jediný sigmoidální neuron se dvěma vstupy se učí pravdivostní tabulku AND
pomocí MSE a BGD. Běh má 10000 epoch a rychlost učení 0,2.
Po trénování Runner vypíše spojité výstupy na téže tabulce.
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
