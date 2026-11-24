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
                        ├── VIN de los VL53L0X
                        └── VCC del MPU6050 (opcional)
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
| Habilitar driver | 5 V (o un pin libre) | DRV8833 nSLEEP / EEP (no si J1 está soldado) |
| Falla del driver (opcional) | D11 (INPUT_PULLUP) | DRV8833 ULT / nFAULT |
| Voltaje de batería | A6 | Divisor 10 k + 10 k (ver arriba) |
| I2C SDA | A4 | SDA de los 3 VL53L0X y del MPU6050 |
| I2C SCL | A5 | SCL de los 3 VL53L0X y del MPU6050 |
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

Queda libre A7 (y D11 si no usas el pin de falla del driver).

La alimentación de cada sensor (VCC, GND) está en la sección siguiente.

## Sensores: cableado pin por pin

La tabla de arriba solo dice a qué pin del Nano va la señal. Cada sensor lleva
además su alimentación. Todos los VCC/VIN van al riel de **5 V** y todos los
GND al riel de **GND** de la protoboard.

### TCRT5000 (módulo de 4 pines: VCC, GND, DO, AO)

| Sensor | VCC | GND | AO | DO |
|---|---|---|---|---|
| Frontal izquierdo (FI) | Riel 5 V | Riel GND | A0 | Sin conectar |
| Frontal derecho (FD) | Riel 5 V | Riel GND | A1 | Sin conectar |
| Trasero izquierdo (TI) | Riel 5 V | Riel GND | A2 | Sin conectar |
| Trasero derecho (TD) | Riel 5 V | Riel GND | A3 | Sin conectar |

- **AO** es la salida analógica: da un valor alto sobre el dojo negro y bajo
  sobre la línea blanca. El umbral se calibra en el dojo real.
- **DO** es la salida digital que compara contra el trimpot azul del módulo. No
  la usamos, así que queda al aire. Tampoco hace falta mover el trimpot.
- Si el módulo trae un LED rojo de encendido, debe prender en cuanto le llegan
  5 V y GND. Si no prende, revisa esos dos cables antes que nada.

### VL53L0X (VIN, GND, SCL, SDA, GPIO1, XSHUT)

| Sensor | VIN | GND | SDA | SCL | XSHUT | GPIO1 |
|---|---|---|---|---|---|---|
| Izquierdo | Riel 5 V | Riel GND | A4 | A5 | D7 | Sin conectar |
| Central | Riel 5 V | Riel GND | A4 | A5 | D8 | Sin conectar |
| Derecho | Riel 5 V | Riel GND | A4 | A5 | D12 | Sin conectar |

- **SDA y SCL son compartidos**: los tres SDA van juntos a A4 y los tres SCL
  juntos a A5. En la protoboard, usa una fila para SDA y otra para SCL, con un
  cable de cada sensor y uno al Nano.
- **XSHUT es propio** de cada sensor (D7, D8, D12). Nunca los unas entre sí.
- VIN va a 5 V porque estas placas traen su propio regulador a 2.8 V. Si tu
  placa solo tiene un pin **VCC** y no dice VIN, revisa si tiene regulador (un
  componente chico de 3 o 5 patas junto al conector). Si no lo tiene, aliméntala
  con el **3V3** del Nano, no con 5 V.
- Los XSHUT se manejan como se explica en la sección de direcciones I2C, más
  abajo: nunca en HIGH.

### Cómo poner los módulos en la protoboard

En la protoboard, los 5 agujeros de cada fila numerada (a–e, y f–j del otro
lado del canal) están unidos por dentro. Por eso:

- **Coloca el módulo con sus pines a lo largo de la tabla**, de modo que cada
  pin quede en una **fila numerada distinta**. Si pones los 4 pines del TCRT5000
  (o los 6 del VL53L0X) en la misma fila, quedan todos en corto: VCC con GND, y
  la señal pegada a los dos.
- Después, desde cada fila sacas un cable: la fila del VCC al riel rojo, la del
  GND al riel azul y la de AO (o SDA, SCL, XSHUT) a su pin del Nano.
- En muchas protoboards los rieles largos están **cortados a la mitad**
  (la línea roja o azul se interrumpe). Si usas las dos mitades, une cada riel
  con un puente.
- El riel de 5 V se alimenta desde el pin **5V** del Nano y el riel de GND desde
  un pin **GND** del Nano. En el banco de pruebas basta con el USB. Cuando
  conectes el buck, su salida va al mismo riel de 5 V.
- Antes de darle corriente, mide con el multímetro en continuidad entre el riel
  de 5 V y el de GND: no debe pitar.

### MPU6050 (GY-521: VCC, GND, SCL, SDA, XDA, XCL, AD0, INT), opcional

| VCC | GND | SDA | SCL | AD0 | XDA, XCL, INT |
|---|---|---|---|---|---|
| Riel 5 V | Riel GND | A4 | A5 | GND o al aire | Sin conectar |

- Comparte el bus I2C con los VL53L0X. Su dirección es **0x68** (AD0 en GND),
  así que no choca con 0x29 ni con 0x30–0x32.
- El GY-521 trae regulador a 3.3 V, por eso va a 5 V. Si tu placa no lo trae,
  aliméntala con el 3V3 del Nano.
- Va plano, cerca del centro, con la flecha X hacia adelante y bien fijo.
- Usa solo uno; el segundo queda de repuesto.

Cableado paso a paso y sketch de prueba en
[`firmware/pruebas/`](../firmware/pruebas/README.md).

## Salidas del DRV8833

Guía completa de los motores, con diagrama: [`motores.md`](motores.md).

| Salida | Va a |
|---|---|
| AOUT1, AOUT2 | Motor izquierdo |
| BOUT1, BOUT2 | Motor derecho |

Si un motor gira al revés, invierte sus dos cables (o invierte el sentido en el código).

Pon un capacitor cerámico de 100 nF entre las dos terminales de cada motor.
Los cables de batería y motores van directos a la placa del DRV8833, no por la
protoboard. Cableado paso a paso, cómo revisar si el DRV8833 alcanza para tus
motores y sketch de prueba en [`firmware/pruebas/`](../firmware/pruebas/README.md).

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
