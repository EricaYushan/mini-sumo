# Pruebas por componente

Un sketch por pieza, para probar cada una sola en el banco antes de juntar todo.
Los pines son los de [`hardware/conexiones.md`](../../hardware/conexiones.md).

| Sketch | Qué prueba | Librerías |
|---|---|---|
| [`prueba_motores/`](prueba_motores/prueba_motores.ino) | DRV8833 y los 2 motores: adelante, atrás, giros, frenado | Ninguna |
| [`prueba_mpu6050/`](prueba_mpu6050/prueba_mpu6050.ino) | MPU6050: bus I2C, aceleración, giro y detección de choques | Ninguna (solo `Wire`) |

En el IDE de Arduino: placa **Arduino Nano**. Si no sube, prueba con
*Procesador: ATmega328P (Old Bootloader)*, que es lo normal en los clones.

---

## 1. Motores (DRV8833)

### Antes de cablear

**¿De qué tamaño son los motores?** Esto decide si el DRV8833 alcanza.
El DRV8833 da ≈1.5 A continuos por canal. Si un motor pide más cuando está
trabado (empujando al rival sin avanzar), el driver se apaga solo por
protección y vuelve a prender: el robot empuja a tirones.

Mídelo así, con el motor desconectado:

1. Multímetro en Ω, escala más baja. Junta las puntas y anota lo que marca
   (la resistencia de los cables, ≈0.2–0.5 Ω).
2. Mide entre las dos terminales del motor. Gira un poco el eje con la mano y
   repite 3 o 4 veces; quédate con el valor **más bajo**, menos lo del paso 1.
3. Corriente con el motor trabado ≈ **8.4 V ÷ R**.

| R del motor | Corriente trabado (LiPo llena) | ¿Alcanza el DRV8833? |
|---|---|---|
| más de 8 Ω | menos de ≈1 A | Sí, con margen. Lo típico de un N20. |
| 5.6 a 8 Ω | 1 a 1.5 A | Justo. Puede cortarse en empujes largos. |
| menos de 5.6 Ω | más de 1.5 A | No. Lo típico de un motor de 25 o 37 mm. |

Si no alcanza, la salida más barata es usar **las dos placas DRV8833, una por
motor**, en modo paralelo (IN1+IN3 juntos, IN2+IN4 juntos, OUT1+OUT3 juntos y
OUT2+OUT4 juntos): así cada motor tiene ≈3 A. El costo es que te quedas sin
driver de repuesto. Pero si los motores son de 25 o 37 mm, el problema mayor es
el tamaño y el peso (80–200 g cada uno en un robot de 10 × 10 cm y 500 g), no
el driver.

### Material

- DRV8833, los 2 motores, la LiPo 2S, el switch.
- 1 capacitor electrolítico de **100–470 µF y 16 V o más**.
- 2 capacitores cerámicos de **100 nF** (dicen "104"), uno por motor. Opcionales
  pero recomendados: reducen el ruido que reinicia al Nano y estorba al I2C.
- Algo para tener el robot **con las ruedas en el aire**.

### Cableado paso a paso

Hazlo todo **con la batería desconectada y el switch apagado**.

1. **Revisa el puente J1 de la placa.** Multímetro en continuidad entre **EEP**
   y **VCC** del DRV8833.
   - Si **pita**: J1 está soldado y EEP ya queda habilitado desde VCC. **No
     conectes EEP a nada.**
   - Si **no pita**: en el paso 5 conectarás EEP al riel de 5 V.
2. **Tierras.** DRV8833 **GND** → riel **GND** de la protoboard. Nano **GND** →
   riel **GND**. Todas las tierras unidas; sin esto los motores no responden o
   se mueven solos.
3. **Señales del motor izquierdo.** Nano **D5** → **IN1**. Nano **D6** → **IN2**.
4. **Señales del motor derecho.** Nano **D9** → **IN3**. Nano **D10** → **IN4**.
5. **Habilitar.** Solo si J1 no pitó en el paso 1: **EEP** → riel **5 V**.
6. **Motores.** Motor izquierdo → **OUT1** y **OUT2**. Motor derecho →
   **OUT3** y **OUT4**. Suelda un capacitor de 100 nF entre las dos terminales
   de cada motor, pegado al motor.
7. **Capacitor grande.** El de 100–470 µF entre **VCC** y **GND** del DRV8833,
   lo más cerca posible de la placa. **Respeta la polaridad**: la pata corta (o
   la franja con "−") va a GND. Al revés puede reventar.
8. **Batería.** LiPo (+) → switch → **VCC** del DRV8833. LiPo (−) → **GND** del
   DRV8833. **Nunca** lleves la batería al riel de 5 V ni al pin 5V del Nano.
9. **Opcional: pin de falla.** **ULT** → Nano **D11**. ULT se pone en LOW cuando
   el DRV8833 se apaga por sobrecorriente o temperatura, y el sketch lo avisa.
   Antes de conectarlo, con la batería conectada y el Nano **sin** conectar a
   ULT, mide el voltaje entre ULT y GND: si marca más de 5 V, la placa lo jala
   a la batería y **no** debes conectarlo al Nano.

La corriente de los motores (batería → VCC → OUT → motores) **no debe pasar por
la protoboard**: sus contactos aguantan ≈1 A y se calientan. Esos cables van
directos o soldados a la placa del DRV8833. Por la protoboard solo pasan las
señales (D5, D6, D9, D10, ULT), el GND común y el 5 V de EEP.

Para el banco de pruebas, **el Nano se alimenta por USB** y la batería solo
alimenta al DRV8833. Así no necesitas el buck todavía y ves el Monitor Serie.

### Antes de prender

- Continuidad entre **VCC** y **GND** del DRV8833: no debe pitar.
- Continuidad entre **VCC** del DRV8833 y el riel de **5 V**: no debe pitar.
- Ruedas en el aire.

### Probar

1. Sube `prueba_motores.ino` con el switch apagado.
2. Abre el Monitor Serie a 115200. Debe decir `=== INICIO ===`.
3. Prende el switch.
4. Escribe `i`: solo debe girar la rueda **izquierda**, hacia adelante.
   Luego `o`: solo la **derecha**, hacia adelante. Si una gira al revés,
   cambia `INVERTIR_IZQ` o `INVERTIR_DER` a `true` en el sketch (o invierte
   los dos cables de ese motor). Si con `i` gira la derecha, los motores están
   cruzados.
5. Prueba `w`, `s`, `a`, `d`, `x` y `b`, y cambia la velocidad con `1` a `9`.
6. Escribe `p` para la rutina automática.
7. Con la velocidad en `9`, frena con la mano una rueda unos 2 segundos (con
   cuidado). Si con `f` aparece `FALLA` o la rueda se corta a tirones, el
   motor pide más de lo que da el DRV8833 (ver la tabla de arriba).

### Si el Nano se reinicia

Se nota porque aparece otra vez `=== INICIO ===` mientras los motores se mueven,
o el LED de la placa parpadea como al subir un programa.

Con el Nano por USB en el banco casi no pasa. **Pasa en el robot sin USB**,
cuando el Nano se alimenta de la misma batería que los motores: al arrancar,
frenar o invertir, el voltaje cae por un instante y el Nano se reinicia en
plena pelea. Por eso el plan pide el **regulador buck a 5 V**. El sketch ya
hace una pausa de 120 ms antes de invertir (`PAUSA_INVERSION_MS`), que ayuda,
pero no reemplaza al buck.

Mientras no tengas el buck, puedes alimentar el Nano desde la LiPo por su pin
**VIN** (tiene regulador propio y acepta 7–12 V). Funciona para pruebas, pero
es más propenso a reiniciarse, y ese regulador se calienta si además alimenta
los 4 TCRT5000, los 3 VL53L0X y el MPU6050. No lo dejes así para competir.

---

## 2. IMU (MPU6050, módulo GY-521)

### ¿Hace falta?

**No es obligatorio**: el reglamento no lo pide y el plan lo deja fuera de la
v1. Puede servir para tres cosas, en este orden de utilidad:

1. **Giros exactos.** Con el giroscopio el robot puede girar 180° de verdad al
   escapar del borde, en vez de "girar 300 ms" y esperar que salga bien
   (eso cambia con la batería y el piso).
2. **Detectar que te empujan**: mandas avanzar y la aceleración dice que vas
   para atrás.
3. **Detectar choques** o que te levantaron con la pala.

Si lo usas, **usa solo uno**. El segundo no aporta nada y queda de repuesto.

### Cableado paso a paso

El GY-521 tiene 8 pines: VCC, GND, SCL, SDA, XDA, XCL, AD0, INT.

1. **VCC** → riel **5 V**. El GY-521 trae su regulador a 3.3 V. Si tu placa
   no lo trae (no dice GY-521 y no tiene un componente de 3 patas junto al
   conector), usa el **3V3** del Nano.
2. **GND** → riel **GND**.
3. **SDA** → **A4**. Es el mismo bus que los VL53L0X: va a la misma fila de
   SDA de la protoboard.
4. **SCL** → **A5**, también a la fila compartida de SCL.
5. **AD0** → **GND** (o al aire; la placa ya lo jala a GND). Así su dirección
   es **0x68**, que no choca con los VL53L0X (0x29 de fábrica, 0x30–0x32 ya
   cambiados).
6. **XDA, XCL, INT**: sin conectar.

### Dónde va en el robot

- **Plano**, con el chip hacia arriba, cerca del centro del robot.
- La flecha **X** impresa en la placa apuntando **hacia adelante**.
- Bien fijo (tornillos o cinta doble cara delgada). Si se mueve, mide su
  propio bamboleo y no el del robot.
- Lejos de los cables de los motores. Si el I2C falla cuando giran los
  motores, acorta SDA y SCL y tréñzalos con un cable de GND.

### Probar

1. Sube `prueba_mpu6050.ino` y abre el Monitor Serie a 115200.
2. La lista de I2C debe mostrar **0x68**. Si no aparece nada, revisa VCC, GND
   y que SDA y SCL no estén cruzados.
3. `WHO_AM_I` debe ser 0x68. Si sale 0x70, 0x72 o 0x98, es un clon: para esta
   prueba sirve igual.
4. **No muevas el robot** los primeros 2 segundos (calibración).
5. Quieto y plano: `az` ≈ 1.00 y `ax`, `ay` ≈ 0.
6. Empuja el robot hacia adelante: `ax` debe ser **positivo**. Si es negativo,
   la placa está volteada.
7. Gira el robot 90° a la izquierda a mano: `giro` debe marcar ≈ +90. A la
   derecha, ≈ −90. Unos grados de error y una deriva lenta son normales; para
   giros de un par de segundos sobra.
8. Da un golpe seco al robot: debe aparecer `CHOQUE`. Ajusta
   `UMBRAL_CHOQUE_G` cuando lo pruebes con choques de verdad.
9. Cuando juntes motores e IMU en el firmware, revisa que las lecturas no
   salten demasiado con los motores girando. Si saltan, fija mejor la placa
   y aleja SDA y SCL de los cables de los motores.
