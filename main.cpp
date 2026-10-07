#include <M5Cardputer.h>
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>

// CalcPuter Matemática — M5Stack Cardputer-Adv / ESP32-S3
// Interface compacta; solucionadores independentes para facilitar expansão.

static const int W = 240, H = 135;
static String input;
static int page = 0;
static int menu = 0;
static const int MENU_COUNT = 10;

static void clearScreen() { M5Cardputer.Display.fillScreen(BLACK); }
static void text(int x,int y,const String &s,uint16_t c=WHITE,int size=1){
  M5Cardputer.Display.setTextColor(c); M5Cardputer.Display.setTextSize(size); M5Cardputer.Display.setCursor(x,y); M5Cardputer.Display.print(s);
}
static String fmt(double x){
  if (std::isnan(x)) return "NaN"; if (!std::isfinite(x)) return x>0?"+INF":"-INF";
  char b[32]; snprintf(b,sizeof(b),"%.10g",x); return String(b);
}
static void header(const char* title){ clearScreen(); text(4,4,"CALCPUTER MATH",CYAN,1); text(4,18,title,WHITE,1); M5Cardputer.Display.drawFastHLine(0,31,W,0x7BEF); }

// ---------- Solvers ----------
static String linear(double a,double b){
  if (std::abs(a)<1e-14) return std::abs(b)<1e-14 ? "INFINITAS SOLUCOES" : "SEM SOLUCAO";
  return "x = " + fmt(-b/a);
}
static String quadratic(double a,double b,double c){
  if(std::abs(a)<1e-14) return linear(b,c);
  double d=b*b-4*a*c;
  if(d>1e-14){ double s=sqrt(d); return "x1="+fmt((-b+s)/(2*a))+"\nx2="+fmt((-b-s)/(2*a))+"\nDelta="+fmt(d); }
  if(std::abs(d)<=1e-14) return "x1=x2="+fmt(-b/(2*a))+"\nDelta=0";
  double re=-b/(2*a), im=sqrt(-d)/(2*std::abs(a));
  return "x1="+fmt(re)+"+"+fmt(im)+"i\nx2="+fmt(re)+"-"+fmt(im)+"i\nDelta="+fmt(d);
}
static std::vector<std::complex<double>> cubicRoots(double a,double b,double c,double d){
  std::vector<std::complex<double>> r;
  if(std::abs(a)<1e-14) return {};
  const std::complex<double> I(0,1);
  double A=b/a,B=c/a,C=d/a;
  double p=B-A*A/3.0, q=2*A*A*A/27.0-A*B/3.0+C;
  std::complex<double> D=std::sqrt(std::complex<double>(q*q/4.0+p*p*p/27.0,0));
  std::complex<double> u=std::pow(-q/2.0+D,1.0/3.0);
  std::complex<double> v=std::pow(-q/2.0-D,1.0/3.0);
  if(std::abs(u)>1e-12) v=-p/(3.0*u);
  else if(std::abs(v)>1e-12) u=-p/(3.0*v);
  std::complex<double> omega=(-1.0+std::sqrt(3.0)*I)/2.0;
  r.push_back(u+v-A/3.0);
  r.push_back(omega*u+std::conj(omega)*v-A/3.0);
  r.push_back(std::conj(omega)*u+omega*v-A/3.0);
  return r;
}
static String cubic(double a,double b,double c,double d){
  if(std::abs(a)<1e-14) return quadratic(b,c,d);
  auto r=cubicRoots(a,b,c,d); String s;
  for(int i=0;i<3;i++){
    double re=r[i].real(), im=r[i].imag();
    s += "x"+String(i+1)+"="+fmt(re);
    if(std::abs(im)>1e-8) s += (im>=0?"+":"")+fmt(im)+"i";
    s += "\n";
  } return s;
}

static double polyEval(const std::vector<double>& a,double x){ double y=0; for(double c:a)y=y*x+c; return y; }
static double polyDeriv(const std::vector<double>& a,double x){ double y=0; int n=a.size()-1; for(int i=0;i<n;i++) y=y*x+a[i]*(n-i); return y; }
static String polynomialReal(const std::vector<double>& a){
  if(a.empty()) return "SEM COEFICIENTES";
  int n=a.size()-1; if(n<=3){ if(n==1)return linear(a[0],a[1]); if(n==2)return quadratic(a[0],a[1],a[2]); if(n==3)return cubic(a[0],a[1],a[2],a[3]); }
  // Procura raízes reais por Newton em vários chutes; útil para graus maiores.
  String out="RAIZES REAIS (NUMERICAS)\n"; std::vector<double> roots;
  for(int seed=-100;seed<=100;seed++){
    double x=seed/5.0;
    for(int k=0;k<60;k++){ double f=polyEval(a,x), fp=polyDeriv(a,x); if(std::abs(fp)<1e-12) break; double nx=x-f/fp; if(std::abs(nx-x)<1e-10){x=nx;break;} x=nx; }
    if(std::abs(polyEval(a,x))<1e-6 && std::abs(x)<1e6){ bool dup=false; for(double r:roots)if(std::abs(r-x)<1e-4)dup=true; if(!dup)roots.push_back(x); }
  }
  std::sort(roots.begin(),roots.end()); for(double r:roots) out+="x="+fmt(r)+"\n";
  if(roots.empty()) out+="Nenhuma raiz real encontrada.\n(Use os graus 2/3 para forma fechada.)";
  return out;
}
static String exponential(double a,double b,double c){ // a*b^x=c
  if(a==0 || b<=0 || std::abs(b-1)<1e-14) return "PARAMETROS INVALIDOS";
  if(c/a<=0) return "SEM SOLUCAO REAL";
  return "x = ln("+fmt(c/a)+") / ln("+fmt(b)+")\n= "+fmt(log(c/a)/log(b));
}
static String logarithmic(double a,double b,double c){ // a*log_b(x)=c
  if(a==0 || b<=0 || std::abs(b-1)<1e-14) return "PARAMETROS INVALIDOS";
  double x=pow(b,c/a); return "x = "+fmt(x);
}
static String powerEq(double a,double p,double c){ // a*x^p=c, real principal/general integer-ish handling
  if(std::abs(a)<1e-14) return std::abs(c)<1e-14?"INFINITAS SOLUCOES":"SEM SOLUCAO";
  double q=c/a; if(q<0 && std::abs(p-std::round(p))<1e-12 && ((long long)std::round(p))%2==0) return "SEM SOLUCAO REAL";
  if(std::abs(p)<1e-14) return std::abs(q-1)<1e-12?"TODO x":"SEM SOLUCAO";
  if(q<0){ long long ip=(long long)std::llround(p); if(ip%2) return "x = "+fmt(-pow(-q,1.0/p)); }
  return "x = "+fmt(pow(q,1.0/p));
}
static String system2(double a,double b,double c,double d,double e,double f){
  double D=a*e-b*d, Dx=c*e-b*f, Dy=a*f-c*d;
  if(std::abs(D)<1e-14) return (std::abs(Dx)<1e-14&&std::abs(Dy)<1e-14)?"INFINITAS SOLUCOES":"SEM SOLUCAO";
  return "x="+fmt(Dx/D)+"\ny="+fmt(Dy/D)+"\nD="+fmt(D);
}

// ---------- UI ----------
static void menuScreen(){
  header("EQUACOES");
  const char* names[MENU_COUNT]={"1 Linear","2 Quadratica","3 Cubica","4 Polinomio n","5 Exponencial","6 Logaritmica","7 Potencia","8 Sistema 2x2","9 Funcao/valor","0 Sobre"};
  int first=(menu/6)*6;
  for(int i=0;i<6 && first+i<MENU_COUNT;i++){
    int idx=first+i; text(6,38+i*14,(idx==menu?"> ":"  ")+String(names[idx]),idx==menu?YELLOW:WHITE,1);
  }
  text(145,38,"↑↓ seleciona",GRAY,1); text(145,52,"ENTER abre",GRAY,1); text(145,66,"ESC volta",GRAY,1);
}
static double readNumber(const String& label){
  // In this firmware, forms are edited through the same single-line editor.
  // The prompt is displayed; caller manages sequential input.
  return 0;
}

static void resultScreen(const String& title,const String& r){
  header(title.c_str()); int y=38; String line="";
  for(unsigned i=0;i<r.length();i++){
    char ch=r[i]; if(ch=='\n'||line.length()>38){ text(5,y,line); y+=14; line=""; if(y>124){ text(5,y,"...",GRAY); break; }} else line+=ch;
  }
  if(line.length()&&y<=124) text(5,y,line);
  text(170,119,"ESC",YELLOW,1);
}

static bool parseList(String s,std::vector<double>& v){
  v.clear(); int start=0;
  while(start<(int)s.length()){
    int p=s.indexOf(',',start); if(p<0)p=s.length(); String t=s.substring(start,p); t.trim(); if(!t.length())return false;
    v.push_back(t.toDouble()); start=p+1;
  } return !v.empty();
}
static String prompt(const char* title,const char* hint){
  header(title); text(4,38,hint,GRAY); text(4,58,"> "+input+"_",YELLOW); text(4,110,"ENTER=resolver  ESC=voltar",WHITE); return input;
}

static int form=0; static String answer="";
static void openForm(){ form=menu; input=""; answer=""; page=1; }
static void processForm(){
  // Compact forms: user types comma-separated coefficients in one line.
  // Examples are shown to make each family unambiguous.
  std::vector<double> v;
  bool ok=parseList(input,v);
  String r;
  switch(form){
    case 0: if(v.size()==2)r=linear(v[0],v[1]); else r="Digite a,b para ax+b=0"; break;
    case 1: if(v.size()==3)r=quadratic(v[0],v[1],v[2]); else r="Digite a,b,c para ax²+bx+c=0"; break;
    case 2: if(v.size()==4)r=cubic(v[0],v[1],v[2],v[3]); else r="Digite a,b,c,d para ax³+bx²+cx+d=0"; break;
    case 3: if(v.size()>=2)r=polynomialReal(v); else r="Digite coeficientes do maior grau ao termo constante"; break;
    case 4: if(v.size()==3)r=exponential(v[0],v[1],v[2]); else r="Digite A,B,C para A·B^x=C"; break;
    case 5: if(v.size()==3)r=logarithmic(v[0],v[1],v[2]); else r="Digite A,B,C para A·log_B(x)=C"; break;
    case 6: if(v.size()==3)r=powerEq(v[0],v[1],v[2]); else r="Digite A,p,C para A·x^p=C"; break;
    case 7: if(v.size()==6)r=system2(v[0],v[1],v[2],v[3],v[4],v[5]); else r="Digite a,b,c,d,e,f para ax+by=c / dx+ey=f"; break;
    case 8: r="Digite uma expressao numerica nesta versao.\nMotor de funcoes e graficos fica separado."; break;
    case 9: r="CalcPuter Math\nM5Stack Cardputer-Adv\nSolvers modulares\nGraus 1,2,3,n + exponencial/log/potencia/sistemas"; break;
  }
  resultScreen("RESULTADO",r); page=2;
}

void setup(){
  auto cfg=M5.config(); M5Cardputer.begin(cfg,true); M5Cardputer.Display.setRotation(1); menuScreen();
}
void loop(){
  M5Cardputer.update();
  if(M5Cardputer.Keyboard.isChange()){
    auto k=M5Cardputer.Keyboard.keysState();
    if(page==0){
      bool redraw=false;
      for(auto c:k.word){ if(c=='w' || c=='W'){menu=(menu+MENU_COUNT-1)%MENU_COUNT;redraw=true;} if(c=='s' || c=='S'){menu=(menu+1)%MENU_COUNT;redraw=true;} }
      if(redraw) menuScreen();
      if(k.enter)openForm();
    } else if(page==1){
      if(k.esc){page=0;input="";menuScreen();}
      for(auto c:k.word){ if(input.length()<70) input+=c; }
      if(k.del && input.length()) input.remove(input.length()-1);
      if(k.enter) processForm();
      if(!k.enter&&!k.esc){ const char* hint="coeficientes separados por virgula"; switch(form){case 0:hint="ax+b=0  ->  a,b";break;case 1:hint="ax²+bx+c=0  ->  a,b,c";break;case 2:hint="ax³+bx²+cx+d=0  ->  a,b,c,d";break;case 3:hint="grau n  ->  a_n,...,a_0";break;case 4:hint="A·B^x=C  ->  A,B,C";break;case 5:hint="A·log_B(x)=C  ->  A,B,C";break;case 6:hint="A·x^p=C  ->  A,p,C";break;case 7:hint="ax+by=c ; dx+ey=f  ->  a,b,c,d,e,f";break;} prompt("ENTRADA",hint); }
    } else { if(k.esc){page=1;prompt("ENTRADA","digite coeficientes");} }
  }
  delay(10);
}
