#pragma once

// Resolve o conflito do nome da tecla PI com a macro de matemática do Arduino
#include <Arduino.h>
#undef PI

#include <M5Cardputer.h>
#include "KeyboardADV.h"
#include "MathEngine.h"

class ExatasApp {
  KeyboardADV kb;
  int page=0;
  int selected=0;
  String calcInput;
  String calcAnswer;
  bool redraw=true;

  static constexpr int W=240;
  static constexpr int H=135;

  void title(const char* t){
    M5Cardputer.Display.setTextColor(TFT_WHITE,TFT_BLACK);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(8,6);
    M5Cardputer.Display.print(t);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(8,27);
    M5Cardputer.Display.print("EXATAS M5  |  FN+setas navegam");
  }

  void card(int x,int y,int w,int h,const char* name,bool active){
    uint16_t c=active?TFT_WHITE:TFT_DARKGREY;
    M5Cardputer.Display.drawRoundRect(x,y,w,h,5,c);
    if(active) M5Cardputer.Display.fillRoundRect(x+2,y+2,w-4,h-4,TFT_DARKCYAN);
    M5Cardputer.Display.setTextColor(TFT_WHITE,TFT_BLACK);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(x+8,y+9);
    M5Cardputer.Display.print(name);
  }

  void home(){
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    title("CENTRAL");
    const char* names[]={"CALCULADORA","VISUAL","SOBRE","TESTE"};
    for(int i=0;i<4;i++) card(8+(i%2)*116,38+(i/2)*35,108,29,names[i],selected==i);
    M5Cardputer.Display.setCursor(8,113);
    M5Cardputer.Display.print("ENTER abre   ESC volta");
    redraw=false;
  }

  void visual(){
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    title("VISUAL");
    M5Cardputer.Display.drawRect(8,39,224,73,TFT_CYAN);
    M5Cardputer.Display.drawCircle(42,74,19,TFT_YELLOW);
    M5Cardputer.Display.fillCircle(42,74,7,TFT_YELLOW);
    M5Cardputer.Display.drawLine(75,52,112,95,TFT_GREEN);
    M5Cardputer.Display.drawLine(112,95,150,52,TFT_GREEN);
    M5Cardputer.Display.drawRect(163,53,48,40,TFT_MAGENTA);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(16,103);
    M5Cardputer.Display.print("Tela de teste: formas e posições funcionando");
    redraw=false;
  }

  void about(){
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    title("SOBRE");
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(10,45);
    M5Cardputer.Display.print("EXATAS M5 - Cardputer ADV");
    M5Cardputer.Display.setCursor(10,62);
    M5Cardputer.Display.print("Projeto refeito do zero.");
    M5Cardputer.Display.setCursor(10,78);
    M5Cardputer.Display.print("Interface independente da antiga tela");
    M5Cardputer.Display.setCursor(10,94);
    M5Cardputer.Display.print("matemática.");
    redraw=false;
  }

  void test(){
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    title("TESTE");
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(10,48);
    M5Cardputer.Display.print("Teclas detectadas:");
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10,68);
    char c=kb.printable();
    if(c) M5Cardputer.Display.printf("%c",c); else M5Cardputer.Display.print("-");
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(10,105);
    M5Cardputer.Display.print("Digite qualquer tecla para testar");
    redraw=false;
  }

  void calculator(){
    bool open=true; calcInput="";calcAnswer="";
    while(open){
      M5Cardputer.update();
      if(kb.changed()){
        auto n=kb.navigation(); auto&s=kb.state();
        if(n==NavKey::Escape){open=false;break;}
        if(s.enter){auto r=ExatasMath::evaluate(calcInput);calcAnswer=r.ok?String(r.value,8):"ERRO: "+r.error;}
        else if(s.del){if(calcInput.length())calcInput.remove(calcInput.length()-1);}
        else {char c=kb.printable();if(c)calcInput+=c;}
      }
      M5Cardputer.Display.fillScreen(TFT_BLACK);title("CALCULADORA");
      M5Cardputer.Display.drawRoundRect(8,43,224,29,5,TFT_WHITE);
      M5Cardputer.Display.setTextSize(2);M5Cardputer.Display.setCursor(14,50);M5Cardputer.Display.print(calcInput);
      M5Cardputer.Display.setTextSize(1);M5Cardputer.Display.setCursor(10,82);M5Cardputer.Display.print("RESULTADO: ");M5Cardputer.Display.print(calcAnswer);
      M5Cardputer.Display.setCursor(10,108);M5Cardputer.Display.print("ENTER calcula | DEL apaga | FN+` volta");
      delay(10);
    }
    redraw=true;
  }

public:
  void begin(){
    auto cfg=M5.config();
    M5Cardputer.begin(cfg,true);
    M5Cardputer.Display.setRotation(1);
    kb.begin();
    redraw=true;
  }

  void loop(){
    M5Cardputer.update();
    if(page==0){
      if(redraw)home();
      if(!kb.changed()){delay(8);return;}
      auto n=kb.navigation();auto&s=kb.state();
      if(n==NavKey::Left){selected=(selected+3)%4;redraw=true;}
      else if(n==NavKey::Right){selected=(selected+1)%4;redraw=true;}
      else if(n==NavKey::Up){selected=(selected+2)%4;redraw=true;}
      else if(n==NavKey::Down){selected=(selected+2)%4;redraw=true;}
      else if(n==NavKey::Escape){}
      else if(s.enter){page=selected+1;redraw=true;}
      else if(kb.printable()>='1'&&kb.printable()<='4'){selected=kb.printable()-'1';redraw=true;}
    } else {
      if(page==1)calculator();
      else if(page==2)visual();
      else if(page==3)about();
      else if(page==4)test();
      if(kb.changed() && kb.navigation()==NavKey::Escape){page=0;redraw=true;}
    }
    delay(8);
  }
};
