# EXATAS M5 — Cardputer ADV

Projeto refeito do zero para testar a interface do M5Stack Cardputer ADV.

## O que foi refeito
- `App.h`: interface e navegação novas.
- `main.cpp`: inicialização limpa do novo aplicativo.
- `KeyboardADV.h`: leitura de teclado e navegação Fn + setas.
- `MathEngine.h`: calculadora básica.
- `platformio.ini`: ambiente Cardputer ADV.

## Tela inicial
- 1 / ENTER: Calculadora
- 2 / navegação: Visual
- 3: Sobre
- 4: Teste de teclado

As setas podem ser usadas com Fn conforme a camada KeyboardADV.

## Importante
Este projeto é uma nova base de firmware. Para testar no Cardputer, compile e faça upload do projeto inteiro pelo PlatformIO.
