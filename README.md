# EXATAS Cardputer ADV

Firmware modular para M5Stack Cardputer-Adv (ESP32-S3FN8 / 8 MB flash), inspirado na organização visual de navegação do Bruce, mas com implementação própria.

## Objetivo

Central portátil de Matemática, Física, Química, Biologia e ferramentas de estudo.

O firmware contém a interface, teclado, motores de cálculo e os módulos principais. Conteúdo expansível pode ser colocado no microSD sem recompilar:

- `/EXATAS/notas/` — textos `.txt`
- `/EXATAS/formulas/` — fórmulas e fichas adicionais
- `/EXATAS/cartoes/` — flashcards
- `/EXATAS/diagramas/` — dados de diagramas
- `/EXATAS/modulos/` — módulos de conteúdo reconhecidos pelo firmware
- `/EXATAS/comandos/` — arquivos de comandos para a ponte externa

## Build no GitHub

O projeto usa PlatformIO, com versões fixadas. Isso reduz o risco de uma dependência mudar e quebrar a compilação.

O workflow em `.github/workflows/build.yml` gera o `.bin` e publica o artefato `exatas-cardputer-adv.bin`.

## Hardware

Alvo: M5Stack Cardputer-Adv. O ADV usa teclado TCA8418; o firmware não usa GPIO-matrix do Cardputer antigo.

A camada `KeyboardADV` trata Fn, setas, Esc e demais combinações sem depender de constantes inexistentes da biblioteca.

## Estado atual

Esta primeira entrega estabelece a base compilável e modular. Os motores científicos são separados para poderem crescer sem transformar `main.cpp` em um arquivo monolítico.
