# Conexiones del robot real (Arduino Nano)

> Estos pines son los del robot real. La simulación de Tinkercad usa otros
> (D7, D8, D11, D12, A4 y A5 van a los ultrasónicos). Al pasar el firmware al
> robot, toma los pines de esta tabla, no los del sketch de simulación.

## Alimentación

```
LiPo 2S (+) ── Switch ──┬── DRV8833 VM (motores, 7.4–8.4 V)
                        └── Buck 5 V IN
Buck 5 V OUT ───────────┬── Nano 5V
                        ├── VCC de los TCRT5000
                        └── VIN de los VL53L0X
Todas las tierras (LiPo −, buck, DRV8833, Nano y sensores) van unidas.
```

Pon un capacitor de 100–470 µF entre VM y GND, cerca del DRV8833.

**Nuestra placa DRV8833** (la negra con pines IN1–IN4, VCC, GND, EEP, OUT1–OUT4 y
ULT) no tiene pin VM: ahí se llama **VCC**, y es la entrada de la batería, no
de 5 V. Las equivalencias son AIN1/AIN2 = IN1/IN2, BIN1/BIN2 = IN3/IN4,
AOUT = OUT1/OUT2, BOUT = OUT3/OUT4 y ULT = nFAULT. Si el puente **J1** está
soldado, EEP queda unido a VCC: en ese caso no conectes EEP al 5 V del Nano
(juntarías la batería con el rail de 5 V). Revisa con el multímetro en
continuidad entre EEP y VCC antes de cablear.

### Antes de conectar el buck

**Ajusta el buck a 5.0 V con el multímetro antes de conectarle nada.** Los
módulos ajustables (Mini-360, MP1584 y similares) traen el trimpot en cualquier
posición, y su salida puede llegar casi al voltaje de la batería (≈8.4 V con la
2S cargada). Como el buck va directo al pin 5V del Nano, sin regulador de por
medio, un voltaje alto quema el ATmega328P (máximo absoluto 6 V) y puede dañar
los sensores. Pasos:

1. Buck conectado solo a la batería, con la salida al aire.
2. Gira el trimpot hasta leer 4.9–5.1 V en la salida.
3. Pon una gota de barniz o pegamento en el trimpot para que no se mueva con
   los golpes.
4. Vuelve a medir y entonces conecta el Nano y los sensores.

Un buck de salida fija a 5 V evita este paso.

### Switch

Por el switch pasa toda la corriente, también la de los dos motores atorados al
empujar. Usa uno de **3 A o más** en DC. Los switches deslizables chicos (tipo
SS12D00) aguantan alrededor de 0.3–0.5 A: se calientan, meten caída de voltaje
y el Nano se puede reiniciar a media pelea.

### Medir la batería (A6)

```
LiPo (+) después del switch ── 10 kΩ ──┬── 10 kΩ ── GND
                                      └── A6
```

Con la LiPo llena (8.4 V) A6 recibe 4.2 V, dentro del rango del ADC.
Voltaje de la batería ≈ `analogRead(A6) × 5.0 / 1023 × 2`. La medida depende de
que el buck dé 5.0 V exactos, así que compárala una vez con el multímetro.

- Por debajo de **≈6.6 V** (3.3 V por celda) conviene dejar de pelear y cargar.
  Ahí el robot ya empuja menos.
- No la bajes de **6.0 V** (3.0 V por celda): ahí la LiPo se daña.
- Esto mide el total, no cada celda. El balance de las celdas lo revisa el
  cargador balanceador.

## Pines

| Función | Pin Nano | Va a |
|---|---|---|
| Motor izquierdo, entrada 1 | D5 (PWM) | DRV8833 AIN1 |
| Motor izquierdo, entrada 2 | D6 (PWM) | DRV8833 AIN2 |
| Motor derecho, entrada 1 | D9 (PWM) | DRV8833 BIN1 |
| Motor derecho, entrada 2 | D10 (PWM) | DRV8833 BIN2 |
| Habilitar driver | 5 V (o un pin libre) | DRV8833 nSLEEP / EEP |
| Voltaje de batería | A6 | Divisor 10 k + 10 k (ver arriba) |
| I2C SDA | A4 | SDA de los 3 VL53L0X |
| I2C SCL | A5 | SCL de los 3 VL53L0X |
| XSHUT del VL53L0X izquierdo | D7 | XSHUT |
| XSHUT del VL53L0X central | D8 | XSHUT |
| XSHUT del VL53L0X derecho | D12 | XSHUT |
| TCRT5000 frontal izquierdo | A0 | AO |
| TCRT5000 frontal derecho | A1 | AO |
| TCRT5000 trasero izquierdo | A2 | AO |
| TCRT5000 trasero derecho | A3 | AO |
| Pulsador de inicio | D2 | Pulsador a GND (INPUT_PULLUP) |
| Estrategia (opcional) | D3, D4 | DIP switch a GND |
| LED de estado | D13 | LED integrado |

Quedan libres A7 y D11.

## Salidas del DRV8833

| Salida | Va a |
|---|---|
| AOUT1, AOUT2 | Motor izquierdo |
| BOUT1, BOUT2 | Motor derecho |

Si un motor gira al revés, invierte sus dos cables (o invierte el sentido en el código).

### PWM distinto en cada motor

En el Nano, D5/D6 trabajan a ≈980 Hz (Timer0) y D9/D10 a ≈490 Hz (Timer1). Con
el mismo `analogWrite` los dos motores pueden ir a velocidades un poco
distintas. Casi siempre pesa más la diferencia entre los propios motores, así
que en los dos casos la solución es la misma: si el robot se desvía en línea
recta, mide y compensa con un factor por motor en el código. No cambies la
frecuencia de Timer0, porque de él dependen `millis()` y `delay()`.

## VL53L0X: direcciones I2C

Los tres arrancan con la dirección 0x29. Al iniciar el programa:

1. Apagar los tres: D7, D8 y D12 como `OUTPUT` y en `LOW`.
2. Soltar D7 (`pinMode(7, INPUT)`), esperar ≈10 ms y asignar al sensor la dirección 0x30.
3. Soltar D8 y asignar 0x31.
4. Soltar D12 y asignar 0x32.

**Nunca pongas XSHUT en HIGH desde el Nano.** El pin del sensor aguanta como
máximo ≈3.6 V y la placa ya lo jala a 2.8 V con su propia resistencia. En
`INPUT` el Nano deja de jalarlo y el sensor prende solo; con `OUTPUT` + `LOW`
se apaga. Así lo indica Pololu para su placa, y en los GY-530 genéricos aplica
lo mismo. No uses `INPUT_PULLUP` en estos pines, porque eso los sube a 5 V.

```cpp
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor[3];
const int XSHUT[3] = {7, 8, 12};
const uint8_t DIR[3] = {0x30, 0x31, 0x32};

Wire.begin();
for (int i = 0; i < 3; i++) { pinMode(XSHUT[i], OUTPUT); digitalWrite(XSHUT[i], LOW); }
delay(10);
for (int i = 0; i < 3; i++) {
  pinMode(XSHUT[i], INPUT);  // soltar: el sensor prende
  delay(10);
  sensor[i].init();
  sensor[i].setAddress(DIR[i]);
}
```

La librería `VL53L0X` de Pololu tiene `setAddress()` para esto.

Si tus VL53L0X son GY-530 genéricos, revisa también el bus I2C: algunos jalan
SDA y SCL a 2.8 V, y el Nano necesita ≈3.5 V para leer un HIGH confiable. Si
el I2C falla de forma intermitente, prueba con resistencias de 4.7 kΩ de SDA y
SCL a 3.3 V o con un adaptador de niveles. La placa de Pololu ya trae
adaptador de niveles.
