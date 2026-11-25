# 002-perceptron

Druhý experiment zkoumá model jediného neuronu. Pro úlohu AND má
při překladu pevně dva vstupy a dvě váhy. Zvolenou aktivací je sigmoida.
První úlohou je logická funkce AND se ztrátou MSE a dávkovým
učením BGD. První běh používá všechny čtyři kombinace,
10 epoch a rychlost učení 0,01. Po učení Runner vyhodnotí
stejnou tabulku; samostatná validace zde není plánována.

[Specifikace experimentu](spec/specifikace-perceptron.md)
