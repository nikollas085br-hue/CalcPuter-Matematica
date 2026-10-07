#pragma once
#include <Arduino.h>
#include <cmath>
#include <functional>

namespace ExatasMath {
struct EvalResult { bool ok; double value; String error; };
class Parser {
  const char* p=nullptr; String err;
  static constexpr double PI=3.14159265358979323846;
  double expr(){double v=term();while(*p=='+'||*p=='-'){char o=*p++;double r=term();v=o=='+'?v+r:v-r;}return v;}
  double term(){double v=power();while(*p=='*'||*p=='/'){char o=*p++;double r=power();if(o=='/'&&r==0){err="divisão por zero";return NAN;}v=o=='*'?v*r:v/r;}return v;}
  double power(){double v=unary();if(*p=='^'){++p;double r=power();v=std::pow(v,r);}return v;}
  double unary(){if(*p=='+'){++p;return unary();}if(*p=='-'){++p;return -unary();}return primary();}
  double number(){char*e=nullptr;double v=strtod(p,&e);if(e==p){err="número esperado";return NAN;}p=e;return v;}
  bool name(String&n){if(!isalpha((unsigned char)*p))return false;n="";while(isalpha((unsigned char)*p)||isdigit((unsigned char)*p))n+=*p++;return true;}
  double primary(){
    if(*p=='('){++p;double v=expr();if(*p==')')++p;else err="parêntese ausente";return v;}
    if(isdigit((unsigned char)*p)||*p=='.')return number();
    String n;if(name(n)){if(n=="pi")return PI;if(n=="e")return M_E;if(*p!='('){err="função sem argumento";return NAN;}++p;double x=expr();if(*p==')')++p;else err="parêntese ausente";
      if(n=="sin")return sin(x);if(n=="cos")return cos(x);if(n=="tan")return tan(x);if(n=="sqrt")return sqrt(x);if(n=="abs")return fabs(x);if(n=="ln")return log(x);if(n=="log")return log10(x);if(n=="exp")return exp(x);err="função desconhecida: "+n;return NAN;}
    err="entrada inválida";return NAN;
  }
public:
  EvalResult evaluate(const String&in){p=in.c_str();err="";double v=expr();while(*p==' ')++p;if(*p!='\0'&&err.length()==0)err="caractere inesperado";return {err.length()==0&&isfinite(v),v,err};}
};
inline EvalResult evaluate(const String&s){Parser p;return p.evaluate(s);}
}
