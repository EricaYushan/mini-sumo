# Circuito del MiniSumo en Tinkercad

Tinkercad Circuits no tiene todas nuestras piezas, así que en la simulación se usan equivalentes. La lógica es la misma que en el robot real, pero los pines de los sensores de rival no: en Tinkercad D7, D8, D11, D12, A4 y A5 van a los ultrasónicos, y en el robot D7, D8 y D12 son los XSHUT y A4/A5 el bus I2C. Para el robot usa los pines de [conexiones reales](../../hardware/conexiones.md), no copies el sketch tal cual.

| Pieza real | En Tinkercad | Por qué sirve |
|---|---|---|
| Arduino Nano | **Arduino Uno R3** | Mismo microcontrolador (ATmega328P) y mismos números de pin |
| DRV8833 | **L293D** | También usa dos entradas por motor (adelante y atrás) |
| Motores 12 V | **Motorreductor de hobby** (Hobby Gearmotor) ×2 | Muestra las rpm y el sentido de giro |
| VL53L0X ×3 | **Sensor de distancia ultrasónico** (4 pines, HC-SR04) ×3 | Tinkercad no tiene sensores I2C de distancia |
| TCRT5000 ×4 | **Potenciómetro** ×4 | Girado a 0 = línea blanca; girado al máximo = dojo negro |
| LiPo 2S 7.4 V | **Batería de 9 V** | No hay LiPo en Tinkercad |
| Switch | **Interruptor deslizante** | On/off |
| Pulsador de inicio | **Pulsador** | Arranque de 5 s |

Vas a necesitar además una placa de pruebas (breadboard) y cables.

## Paso a paso

### 1. Alimentación

1. Pon la **batería de 9 V** y el **interruptor deslizante**.
2. Batería (+) → pata común del interruptor.
3. Otra pata del interruptor → riel **rojo de abajo** de la placa de pruebas (este es el riel de motores, 9 V).
4. Batería (−) → riel **azul de abajo**.
5. Arduino **5V** → riel **rojo de arriba** (riel lógico de 5 V).
6. Arduino **GND** → riel **azul de arriba**.
7. Une los dos rieles azules (GND común). **Si no, nada funciona.**
8. Riel rojo de abajo (9 V) → Arduino **Vin**, para que el Arduino se alimente de la batería.

### 2. Driver L293D y motores

Pon el L293D en el centro de la placa de pruebas, cruzando la ranura. La muesca queda a la izquierda, así que el pin 1 es el de abajo a la izquierda.

| Pin L293D | Nombre en Tinkercad | Va a |
|---|---|---|
| 1 | Enable 1&2 | 5 V (riel rojo de arriba) |
| 2 | Input 1 | Arduino **D5** |
| 3 | Output 1 | Motor izquierdo, terminal 1 |
| 4, 5 | Ground | GND |
| 6 | Output 2 | Motor izquierdo, terminal 2 |
| 7 | Input 2 | Arduino **D6** |
| 8 | Power 2 | 9 V (riel de motores) |
| 9 | Enable 3&4 | 5 V |
| 10 | Input 3 | Arduino **D9** |
| 11 | Output 3 | Motor derecho, terminal 1 |
| 12, 13 | Ground | GND |
| 14 | Output 4 | Motor derecho, terminal 2 |
| 15 | Input 4 | Arduino **D10** |
| 16 | Power 1 | 5 V |

### 3. Sensores de rival (3 ultrasónicos)

Ponlos en fila: izquierda, centro y derecha. Cada uno: **VCC → 5 V** y **GND → GND**.

| Sensor | TRIG | ECHO |
|---|---|---|
| Izquierdo | D7 | D8 |
| Central | D11 | D12 |
| Derecho | A4 | A5 |

### 4. Sensores de borde (4 potenciómetros)

Cada potenciómetro: una pata lateral a **5 V**, la otra a **GND** y la del **medio** (wiper) al pin analógico.

| Sensor | Pin |
|---|---|
| Frontal izquierdo | A0 |
| Frontal derecho | A1 |
| Trasero izquierdo | A2 |
| Trasero derecho | A3 |

### 5. Pulsador de inicio y LED

- **Pulsador:** una pata a **D2** y la del lado opuesto a **GND**. No necesita resistencia (el código usa `INPUT_PULLUP`).
- **LED (opcional, para ver el parpadeo):** ánodo (pata larga) → resistencia de 220 Ω → **D13**; cátodo → GND.

### 6. Código

1. Clic en **Código** → **Texto** (confirma que se borran los bloques).
2. Pega el contenido de [`minisumo_sim/minisumo_sim.ino`](minisumo_sim/minisumo_sim.ino).
3. Abre el **Monitor en serie** para ver en qué estado está el robot.

## Cómo probarlo

1. **Iniciar simulación.** Gira los 4 potenciómetros **al máximo** (dojo negro).
2. Presiona el pulsador. El LED parpadea durante 5 s y los motores **no se mueven** (regla de arranque).
3. **SEARCH:** sin nada cerca, los motores giran en sentidos opuestos (el robot gira en el lugar).
4. **ATTACK:** haz clic en el ultrasónico central y acerca el objeto a menos de 60 cm. Los dos motores van adelante a tope.
5. **TRACK:** acerca el objeto solo al sensor izquierdo o al derecho. Un motor va más rápido que el otro para corregir el rumbo.
6. **EDGE_ESCAPE:** gira un potenciómetro frontal **a 0** (línea blanca). Los motores retroceden y después giran, aunque haya rival. El borde siempre tiene prioridad.

## Diferencias con el robot real

- El L293D pierde ≈1.5–2 V; el DRV8833 casi nada. En Tinkercad las rpm salen menores que en la realidad.
- Los HC-SR04 se leen por pulsos; los VL53L0X, por I2C. En el robot real cambian `leerRival()`, las constantes de pines de los sensores de rival y su parte de `setup()`.
- Los TCRT5000 reales hay que calibrarlos sobre el dojo. El `UMBRAL_BLANCO = 300` es solo para la simulación.
