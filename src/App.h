#pragma once
#include <M5Cardputer.h>
#include "KeyboardADV.h"
#include "MathEngine.h"

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

class ExatasApp {
  KeyboardADV kb;
  WebServer server{80};

  int menu = 0;
  String input;
  String answer;

  bool webStarted = false;
  bool lastKeyState = false;

  const char* icons[18] = {
    "CALC","EQ","GRAF","MAT","TRIG","GEO","FIS","QUI","BIO",
    "TAB","EST","REL","BIB","ENV","CEL","ADD","ATLH","CONF"
  };

  // ------------------------------------------------------------
  // Wi-Fi local do Cardputer.
  // O celular pode conectar diretamente nesta rede.
  // ------------------------------------------------------------
  const char* AP_NAME = "EXATAS-M5";
  const char* AP_PASS = "12345678";

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

  // ------------------------------------------------------------
  // Página que abre no celular.
  // Ela permite editar texto/C++ e salvar no LittleFS.
  //
  // IMPORTANTE:
  // salvar C++ NÃO transforma C++ em firmware.
  // O Cardputer não possui um compilador C++ embutido.
  // ------------------------------------------------------------
  String htmlPage() {
    String code = "";

    if (LittleFS.exists("/editor.cpp")) {
      File f = LittleFS.open("/editor.cpp", "r");
      if (f) {
        code = f.readString();
        f.close();
      }
    }

    code.replace("&", "&amp;");
    code.replace("<", "&lt;");
    code.replace(">", "&gt;");

    String html = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta charset="utf-8">
<title>Exatas M5</title>
<style>
body{font-family:Arial,sans-serif;background:#111;color:#eee;margin:0;padding:16px}
main{max-width:800px;margin:auto}
h1{font-size:22px}
textarea{width:100%;height:430px;box-sizing:border-box;background:#050505;color:#eee;
border:1px solid #555;border-radius:8px;padding:12px;font:14px monospace}
button{padding:12px 16px;margin:8px 4px 8px 0;border:0;border-radius:7px;font-weight:bold}
.status{padding:10px;border-radius:7px;background:#222;margin:10px 0}
.small{color:#aaa;font-size:13px}
</style>
</head>
<body>
<main>
<h1>EXATAS CARDPUTER ADV</h1>
<div class="status">Conectado ao M5 pela rede Wi-Fi.</div>

<p class="small">
Este editor envia e salva o código no Cardputer. Para executar uma nova
programação C++, o código ainda precisa ser compilado em firmware.
</p>

<textarea id="code">)HTML";

    html += code;

    html += R"HTML(</textarea>
<br>
<button onclick="saveCode()">SALVAR NO M5</button>
<button onclick="location.reload()">ATUALIZAR</button>
<div id="status" class="status"></div>

<script>
async function saveCode(){
  const code=document.getElementById('code').value;
  const r=await fetch('/save',{
    method:'POST',
    headers:{'Content-Type':'text/plain'},
    body:code
  });
  document.getElementById('status').textContent=await r.text();
}
</script>
</main>
</body>
</html>
)HTML";

    return html;
  }

  void startWeb() {
    if (webStarted)
      return;

    if (!LittleFS.begin(true)) {
      Serial.println("LittleFS: erro");
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_NAME, AP_PASS);

    server.on("/", HTTP_GET, [this]() {
      server.send(200, "text/html; charset=utf-8", htmlPage());
    });

    server.on("/save", HTTP_POST, [this]() {
      String code = server.arg("plain");

      File f = LittleFS.open("/editor.cpp", "w");
      if (!f) {
        server.send(500, "text/plain; charset=utf-8",
                    "Erro ao abrir /editor.cpp");
        return;
      }

      f.print(code);
      f.close();

      server.send(200, "text/plain; charset=utf-8",
                  "Codigo salvo no M5 em /editor.cpp");
    });

    server.on("/status", HTTP_GET, [this]() {
      String msg = "EXATAS-M5 | IP: ";
      msg += WiFi.softAPIP().toString();
      server.send(200, "text/plain; charset=utf-8", msg);
    });

    server.begin();
    webStarted = true;

    Serial.println();
    Serial.println("================================");
    Serial.println("EXATAS CARDPUTER - EDITOR WIFI");
    Serial.print("Rede: ");
    Serial.println(AP_NAME);
    Serial.print("Senha: ");
    Serial.println(AP_PASS);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("================================");
  }

  // ------------------------------------------------------------
  // Calculadora original
  // ------------------------------------------------------------
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
          answer = r.ok
                     ? String(r.value, 10)
                     : "ERRO: " + r.error;
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

public:
  void begin() {
    auto cfg = M5.config();

    M5Cardputer.begin(cfg, true);
    kb.begin();

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);

    // Inicia a rede do editor.
    startWeb();
  }

  void loop() {
    M5Cardputer.update();

    // Mantém o servidor do celular funcionando.
    if (webStarted)
      server.handleClient();

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
      if (menu == 0) {
        calc();
      } else {
        M5Cardputer.Display.fillScreen(TFT_BLACK);
        header();

        M5Cardputer.Display.setCursor(5, 45);
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.print(icons[menu]);

        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setCursor(5, 75);
        M5Cardputer.Display.print("Editor Wi-Fi ativo.");

        delay(900);
      }

      drawHome();
    }
  }
};
