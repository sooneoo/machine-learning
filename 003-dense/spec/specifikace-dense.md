# Specifikace experimentu 003-dense

- **Projekt:** 003-dense
- **Revize:** 1
- **Stav:** návrh

## 1. Cíl

Navázat na experiment [002-perceptron](../../002-perceptron/spec/specifikace-perceptron.md) propojením více neuronů do malé neuronové sítě s plně propojenými vrstvami.

Experiment má dva hlavní cíle:

1. **Ověřit matematiku malé vícevrstvé neuronové sítě.** Popsat a ověřit dopředný výpočet, zpětné šíření gradientů mezi vrstvami a aktualizaci parametrů při trénování.
2. **Zdokonalit architekturu modelu a trénovacího programu.** Navázat na dosavadní implementaci v jazyce C bez frameworku a navrhnout vhodné rozdělení odpovědností mezi model, vrstvy, výpočet ztráty, gradienty a optimalizaci. Cílem je omezit závislost programu na konkrétní topologii sítě, aktivační a ztrátové funkci při zachování jednoduchosti, efektivity a přímé kontroly nad výpočtem.

Ověřovací úlohou bude logická funkce XOR. Na rozdíl od AND není XOR lineárně separovatelná a jediný neuron s afinním vstupem, sigmoidální nebo ReLU aktivací a rozhodovacím prahem ji nedokáže klasifikovat správně na všech čtyřech binárních vstupech. Malá vícevrstvá síť s vhodnými nelineárními aktivacemi tuto reprezentační schopnost má.

Správnost matematického modelu bude ověřena kontrolou analytických gradientů vůči numerickým aproximacím a schopnost trénovacího programu naučit síť bude ověřena na pravdivostní tabulce XOR. AND poslouží jako kontrolní úloha navazující na předchozí experiment.

Naučení pravdivostní tabulky není samo o sobě důkazem generalizace ani výpočetní výhody architektury. Konkrétní topologie, rozhraní, datové struktury, aktivační a ztrátové funkce, experimentální podmínky a měřitelná kritéria úspěchu budou stanoveny v dalších částech specifikace před schválením implementace.
