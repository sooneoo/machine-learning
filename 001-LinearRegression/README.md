# 001-LinearRegression

První experiment se skalární lineární regresí v čistém C.
`Regression` počítá $\hat y=ax+b$, `Optimizer` provádí SGD po
jednotlivých bodech a `run_online_experiment()` v `src/main.c` řídí
deset epoch nad třemi body zjednodušeného převodu kilometrů na míle.

Architektura, rovnice, akceptační scénáře, skutečné výsledky všech
30 kroků a navržený závěr jsou v
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

První běh používá seed 42 a `lr = 0.1f`. Vypisuje po každém bodu ztrátu
před aktualizací a parametry po ní. Pozorovaná ztráta prudce roste;
závěr o použitelnosti konfigurace je ve specifikaci stále jen návrhem.
Varianty s jiným `lr` a s dávkami se teprve navrhnou.
