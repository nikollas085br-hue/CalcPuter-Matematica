#pragma once
#include <M5Cardputer.h>
#include "KeyboardADV.h"
#include "MathEngine.h"

// ============================================================
// EXATAS CARDPUTER ADV - ARQUIVO PRINCIPAL
// Edite este arquivo para mudar a lógica e as telas do projeto.
// ============================================================

class ExatasApp {
  KeyboardADV kb;
  int menu = 0;
  String input;
  String answer;

  const char* icons[18] = {
    "CALC","EQ","GRAF","MAT","TRIG","GEO","FIS","QUI","BIO",
    "TAB","EST","REL","BIB","ENV","CEL","ADD","ATLH","CONF"
  };

  void header() {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(3, 3);
    M5Cardputer.Display.printf(
      "EXATAS  BAT %d%%",
      (int)M5Cardputer.Power.getBatteryLevel()
    );
  }

  void drawHome() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    header();

    M5Cardputer.Display.setTextSize(2);

    for (int i = -2; i <= 2; i++) {
      int idx = (menu + i + 18) % 18;
      int x = 4 + (i + 2) * 47;

      if (i == 0)
        M5Cardputer.Display.drawRect(x - 2, 35, 43, 46, TFT_WHITE);

      M5Cardputer.Display.setCursor(x, 48);
      M5Cardputer.Display.print(icons[idx]);
    }

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(5, 92);
    M5Cardputer.Display.print("< > navegar   ENTER abrir");
  }

  // ==========================================================
  // AQUI FICA A CALCULADORA.
  // Você pode substituir esta função por outra programação.
  // ==========================================================
  void calc() {
    bool run = true;
    input = "";
    answer = "";

    while (run) {
      M5Cardputer.update();

      M5Cardputer.Display.fillScreen(TFT_BLACK);
      header();

      M5Cardputer.Display.setCursor(4, 25);
      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.print("CALCULADORA");

      M5Cardputer.Display.setTextSize(2);
      M5Cardputer.Display.setCursor(4, 45);
      M5Cardputer.Display.print(input);

      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setCursor(4, 75);
      M5Cardputer.Display.print(answer);

      auto &s = kb.state();

      if (kb.changed() && M5Cardputer.Keyboard.isPressed()) {
        NavKey n = kb.navigation();

        if (n == NavKey::Escape) {
          run = false;
          continue;
        }

        if (s.enter) {
          auto r = ExatasMath::evaluate(input);
          answer = r.ok ? String(r.value, 10) : "ERRO: " + r.error;
          continue;
        }

        if (s.del) {
          if (input.length())
            input.remove(input.length() - 1);
          continue;
        }

        char c = kb.printable();
        if (c)
          input += c;
      }

      delay(2);
    }
  }

  // ==========================================================
  // MÓDULO GENÉRICO
  // Troque o conteúdo desta função para criar novas funções.
  // ==========================================================
  void modulo() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    header();

    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(5, 35);
    M5Cardputer.Display.print(icons[menu]);

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(5, 65);
    M5Cardputer.Display.print("MODULO PRONTO PARA PROGRAMAR");

    M5Cardputer.Display.setCursor(5, 80);
    M5Cardputer.Display.print("Edite src/App.h para alterar.");

    delay(1200);
  }

public:
  void begin() {
    auto cfg = M5.config();

    M5Cardputer.begin(cfg, true);
    kb.begin();

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
  }

  void loop() {
    M5Cardputer.update();

    if (!kb.changed()) {
      drawHome();
      delay(3);
      return;
    }

    auto &s = kb.state();
    NavKey n = kb.navigation();

    if (n == NavKey::Left) {
      menu = (menu + 17) % 18;
      drawHome();
      return;
    }

    if (n == NavKey::Right) {
      menu = (menu + 1) % 18;
      drawHome();
      return;
    }

    if (s.enter) {
      if (menu == 0)
        calc();
      else
        modulo();

      drawHome();
    }
  }
};
