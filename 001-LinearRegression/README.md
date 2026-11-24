# 001-LinearRegression

První výzkumný experiment se skalární lineární regresí v C. Projekt je ve fázi přípravy návrhu; trénovací algoritmus ani zdrojový kód modelu dosud nejsou implementovány.

Pracovní návrh, potvrzené předpoklady a otevřená rozhodnutí jsou v [české specifikaci](spec/specifikace-linearni-regrese.md). Implementace začne až po společném dokončení specifikace.

Adresáře `src/`, `include/regression/` a `tests/` jsou zatím prázdné. `Makefile` a konfigurace `config/linux_x86_64.mk` a `config/win32_x86_64.mk` tvoří pouze strukturu. Cíle `build` a `test` na obou platformách záměrně skončí chybou, dokud nevznikne schválená implementace a jednotkové testy. Větev experimentu je `experiment/001-linear-regression`.
