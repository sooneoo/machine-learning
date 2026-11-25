# 001-LinearRegression

První experiment se skalární lineární regresí v čistém C.
`Regression` počítá $\hat y=ax+b$. `Optimizer` podporuje SGD
(aktualizaci po každém bodu) a BGD (aktualizaci po celé trénovací
množině). `src/main.c` porovnává oba postupy při
`lr = 0.01f` a `lr = 0.1f` během deseti epoch nad třemi body
zjednodušeného převodu kilometrů na míle.

Architektura, rovnice, akceptační scénáře, výsledky obou metod a
navržený závěr jsou v
[české specifikaci](spec/specifikace-linearni-regrese.md).

## Překlad a spuštění

Linux x86-64:

```sh
make PLATFORM=linux_x86_64 test
make PLATFORM=linux_x86_64 exec
```

Windows x86-64 s MSYS2 MinGW-w64:

```powershell
mingw32-make PLATFORM=win32_x86_64 test
mingw32-make PLATFORM=win32_x86_64 exec
```

`test` používá sdílené `lib/unit`; produkční program tuto knihovnu
nelinkuje. Cesty k hlavičkám jsou v platformních konfiguracích přes
`LIB_ROOT`. Přesun knihoven vyžaduje změnu překladové cesty, nikoli
direktiv `#include`. Výstupy platforem se ukládají odděleně do
`.cache/<platform>/`.

Každý ze čtyř běhů nastavuje seed 42. SGD vypisuje ztrátu před
každým z 30 kroků, BGD MSE před každým z 10 kroků. SGD prochází
body v pevném pořadí. Specifikace obsahuje tabulky a grafy MSE
po epochách.
Závěr testování čeká na odsouhlasení vlastníkem.
