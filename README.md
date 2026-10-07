# CalcPuter Matemática — M5Stack Cardputer-Adv

Projeto-base para **M5Stack Cardputer-Adv (K132-Adv / ESP32-S3 / 240×135)**.

## Equações implementadas

- Linear: `ax + b = 0`
- Quadrática: `ax² + bx + c = 0`, incluindo raízes complexas
- Cúbica: `ax³ + bx² + cx + d = 0`, incluindo raízes complexas
- Polinômio de grau n: busca numérica de raízes reais para graus maiores
- Exponencial: `A·B^x = C`
- Logarítmica: `A·log_B(x) = C`
- Potência: `A·x^p = C`
- Sistema linear 2×2

## Entrada

Na tela de uma categoria, digite os coeficientes separados por vírgula e pressione ENTER.

Exemplos:

- `2,5` → `2x+5=0`
- `1,-3,2` → `x²-3x+2=0`
- `1,0,-1,0` → `x³-x=0`
- `1,2,8` → `1·2^x=8`
- `1,10,2` → `log_10(x)=2`
- `2,3,16` → `2x³=16` se p=3
- `1,2,5,3,4,6` → sistema 2×2

## Compilação

O repositório usa PlatformIO. O workflow do GitHub Actions compila automaticamente o firmware e publica o `.bin` como artefato.

```bash
pio run -e m5stack-cardputer-adv
```

O arquivo gerado fica em `.pio/build/m5stack-cardputer-adv/firmware.bin`.

> Observação: “qualquer equação” no sentido de um CAS simbólico universal exigiria um motor algébrico muito maior. Este projeto não finge ter isso: as famílias suportadas têm solucionadores reais e independentes, e o polinômio de grau n usa método numérico para graus superiores.
