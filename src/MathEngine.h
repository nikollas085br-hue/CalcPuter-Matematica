#pragma once
#include <Arduino.h>
#include <cmath>
#include <functional>

namespace ExatasMath {

struct EvalResult { bool ok; double value; String error; };

class Parser {
  const char* p = nullptr;
  String err;
  static constexpr double PI = 3.14159265358979323846;
  double expr() { double v=term(); while (*p=='+'||*p=='-') { char o=*p++; double r=term(); v=(o=='+')?v+r:v-r; } return v; }
  double term() { double v=power(); while (*p=='*'||*p=='/') { char o=*p++; double r=power(); if(o=='/'&&r==0){err="divisão por zero";return NAN;} v=(o=='*')?v*r:v/r;} return v; }
  double power() { double v=unary(); if(*p=='^'){++p; double r=power(); v=std::pow(v,r);} return v; }
  double unary() { if(*p=='+'){++p;return unary();} if(*p=='-'){++p;return -unary();} return primary(); }
  double number() { char* e=nullptr; double v=strtod(p,&e); if(e==p){err="número esperado";return NAN;} p=e; return v; }
  bool name(String &n){ if(!isalpha((unsigned char)*p)) return false; n=""; while(isalpha((unsigned char)*p)||isdigit((unsigned char)*p)) n+=*p++; return true; }
  double primary() {
    if(*p=='('){++p;double v=expr();if(*p==')')++p;else err="parêntese ausente";return v;}
    if(isdigit((unsigned char)*p)||*p=='.') return number();
    String n; if(name(n)){ if(n=="pi") return PI; if(n=="e") return M_E; if(*p!='('){err="função sem argumento";return NAN;} ++p; double x=expr(); if(*p==')')++p; else err="parêntese ausente";
      if(n=="sin")return sin(x); if(n=="cos")return cos(x); if(n=="tan")return tan(x); if(n=="asin")return asin(x); if(n=="acos")return acos(x); if(n=="atan")return atan(x); if(n=="sqrt")return sqrt(x); if(n=="abs")return fabs(x); if(n=="ln")return log(x); if(n=="log")return log10(x); if(n=="exp")return exp(x); if(n=="floor")return floor(x); if(n=="ceil")return ceil(x); err="função desconhecida: "+n; return NAN; }
    err="entrada inválida"; return NAN;
  }
public:
  EvalResult evaluate(const String& input){ p=input.c_str(); err=""; double v=expr(); while(*p==' ')++p; if(*p!='\0'&&err.length()==0)err="caractere inesperado"; return {err.length()==0 && isfinite(v),v,err}; }
};

inline EvalResult evaluate(const String& s){ Parser p; return p.evaluate(s); }

inline double factorial(int n){ if(n<0||n>170)return NAN; double r=1; for(int i=2;i<=n;i++)r*=i; return r; }
inline long long gcd(long long a,long long b){a=llabs(a);b=llabs(b);while(b){long long t=a%b;a=b;b=t;}return a;}
inline long long lcm(long long a,long long b){if(!a||!b)return 0;return llabs(a/gcd(a,b)*b);}

struct RootResult { int count; double roots[4]; };
inline RootResult polynomial2(double a,double b,double c){ RootResult r{0,{0,0,0,0}}; if(fabs(a)<1e-14){if(fabs(b)>1e-14)r.roots[r.count++]=-c/b;return r;} double d=b*b-4*a*c;if(d>=0){double s=sqrt(d);r.roots[r.count++]=(-b+s)/(2*a);if(s>1e-14)r.roots[r.count++]=(-b-s)/(2*a);}return r; }
inline double numericRoot(std::function<double(double)> f,double a,double b){double fa=f(a),fb=f(b);if(!isfinite(fa)||!isfinite(fb)||fa*fb>0)return NAN;for(int i=0;i<100;i++){double m=(a+b)/2,fm=f(m);if(!isfinite(fm))return NAN;if(fabs(fm)<1e-10)return m;if(fa*fm<=0){b=m;fb=fm;}else{a=m;fa=fm;}}return (a+b)/2;}

}
