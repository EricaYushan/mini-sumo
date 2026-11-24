# Conectar los motores (DRV8833 + Arduino Nano)

Guía corta solo para los motores. El resto de las conexiones está en
[`conexiones.md`](conexiones.md) y el programa de prueba en
[`firmware/pruebas/prueba_motores/`](../firmware/pruebas/prueba_motores/prueba_motores.ino).

## Diagrama

```
                    ┌──────────── DRV8833 ────────────┐
 Nano D5  ─────────►│ IN1                        OUT1 │──┐
 Nano D6  ─────────►│ IN2                        OUT2 │──┴── Motor IZQUIERDO
 Nano D9  ─────────►│ IN3                        OUT3 │──┐
 Nano D10 ─────────►│ IN4                        OUT4 │──┴── Motor DERECHO
 5 V (*)  ─────────►│ EEP                             │
 (opcional) D11 ◄───│ ULT                             │
 LiPo + ─ switch ──►│ VCC  ═╤═ capacitor 100–470 µF   │
 GND común ─────────│ GND  ═╧═ (pata − a GND)         │
                    └─────────────────────────────────┘
(*) Solo si el puente J1 NO está soldado. Ver el paso 1.
```

## Tabla

| Pin del DRV8833 | Va a | Para qué |
|---|---|---|
| IN1 | Nano **D5** | Motor izquierdo, sentido 1 (PWM) |
| IN2 | Nano **D6** | Motor izquierdo, sentido 2 (PWM) |
| IN3 | Nano **D9** | Motor derecho, sentido 1 (PWM) |
| IN4 | Nano **D10** | Motor derecho, sentido 2 (PWM) |
| EEP | **5 V** (salvo que J1 esté soldado) | Encender el driver |
| VCC | LiPo **+**, después del switch | Fuerza para los motores (6–8.4 V) |
| GND | **GND común** | Tierra |
| OUT1, OUT2 | Las 2 terminales del motor izquierdo | |
| OUT3, OUT4 | Las 2 terminales del motor derecho | |
| ULT | Nano **D11** (opcional) | Avisa si el driver se apaga por falla |

No chocan con nada: D7, D8 y D12 son los XSHUT de los VL53L0X, A0–A3 los
TCRT5000, A4/A5 el I2C (IMU y VL53L0X) y A6 la medición de batería.

## Paso a paso

Todo **con la batería desconectada y el switch apagado**.

1. **Revisa el puente J1.** Multímetro en continuidad entre **EEP** y **VCC**
   del DRV8833.
   - **Pita**: J1 está soldado y el driver ya se enciende solo. **EEP no va a
     ningún lado.** Si lo conectas al 5 V, juntas la batería con el 5 V del
     Nano.
   - **No pita**: EEP va al **5 V**. Sin esto los motores no se mueven (el
     driver queda dormido).
2. **Tierras.** GND del DRV8833, GND del Nano, LiPo **−** y, cuando lo tengas,
   **IN−** y **OUT−** del buck Mini 560: todos unidos.
3. **Señales.** D5 → IN1, D6 → IN2, D9 → IN3, D10 → IN4.
4. **Motores.** Izquierdo en OUT1/OUT2, derecho en OUT3/OUT4. El orden de los
   dos cables de cada motor no importa: si gira al revés se corrige después.
   Si tienes, suelda un capacitor cerámico de 100 nF ("104") entre las dos
   terminales de cada motor: quita ruido que puede trabar el I2C.
5. **Capacitor grande.** 100–470 µF, **16 V o más**, entre VCC y GND del
   DRV8833, lo más pegado posible. La pata corta (franja con "−") a GND.
6. **Batería.** LiPo **+** → switch → **VCC** del DRV8833. La batería **nunca**
   va al pin 5V del Nano.
7. **Revisión con el multímetro** antes de conectar la batería:
   - VCC con GND del DRV8833: **no debe pitar**.
   - VCC del DRV8833 con el 5 V del Nano: **no debe pitar** (salvo que J1 esté
     soldado y hayas conectado EEP, que es justo el error del paso 1).

Los cables de la batería y de los motores van **directos o soldados** al
DRV8833, no por la protoboard (sus contactos aguantan ≈1 A y se calientan).
Por la protoboard solo pasan las señales, el GND y el 5 V de EEP.

## El buck Mini 560 (5 V)

```
LiPo + ─ switch ──┬── VCC del DRV8833
                  └── IN+  Mini 560  OUT+ ── pin 5V del Nano (y riel de 5 V)
LiPo − ───────────┴── IN−            OUT− ── GND
```

- El Mini 560 de 5 V es de **salida fija**: no hay que ajustarle nada. Aun
  así, mide OUT+ antes de conectarlo al Nano: debe dar 4.9–5.1 V.
- OUT+ va al pin **5V** del Nano, **no** a VIN.
- Necesita ≈7 V o más a la entrada para dar 5 V limpios. Con la LiPo llena
  (8.4 V) va bien; cuando baja de ≈7 V la salida empieza a caer, y de todos
  modos ahí ya conviene cargar la batería.
- Mientras no lo tengas: prueba los motores con el Nano por **USB** y la
  batería solo en el DRV8833. Sin buck, el Nano puede reiniciarse a media
  pelea cuando los motores frenan o cambian de sentido.

## Probar

Ruedas en el aire, Nano por USB. Sube
[`prueba_motores.ino`](../firmware/pruebas/prueba_motores/prueba_motores.ino),
abre el Monitor Serie a 115200 y prende el switch.

- `i`: solo debe girar la rueda **izquierda**, hacia adelante.
- `o`: solo la **derecha**, hacia adelante.
- Si una gira al revés, pon `INVERTIR_IZQ` o `INVERTIR_DER` en `true` (o
  voltea los dos cables de ese motor).
- Si con `i` gira la derecha, cruzaste los motores: intercambia OUT1/OUT2 con
  OUT3/OUT4.
- Luego `w`, `s`, `a`, `d`, `x`, velocidades `1`–`9` y la rutina `p`.

Si nada se mueve: revisa EEP (paso 1), el GND común y que el switch esté
prendido. Si con `f` sale `FALLA`, el driver se apagó por corriente o
temperatura.

## Motores de 12 V o N20 de 6 V

El cableado es **el mismo** para los dos. Lo que cambia es el programa:

| Motor | Con la LiPo 2S (7.2 V, 8.4 V llena) | `PWM_MAX` en el sketch |
|---|---|---|
| Los actuales de 12 V | Reciben menos de su voltaje: van más lentos y con menos fuerza, pero no se dañan | `255` |
| N20 de 6 V, 500 rpm | Reciben **más** de su voltaje: se calientan y duran menos | `180` (≈6 V con la batería llena) |

Con N20 de 6 V a 500 rpm y llantas de 34 mm el robot iría a ≈0.9 m/s, más del
doble que con los de 12 V, pero con menos fuerza de empuje. El DRV8833 aguanta
bien un N20; con motores de 25 o 37 mm puede quedarse corto (≈1.5 A por
motor). Cómo medirlo: [`firmware/pruebas/README.md`](../firmware/pruebas/README.md).
