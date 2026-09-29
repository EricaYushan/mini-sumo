# Conexiones del robot real (Arduino Nano)

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

## Pines

| Función | Pin Nano | Va a |
|---|---|---|
| Motor izquierdo, entrada 1 | D5 (PWM) | DRV8833 AIN1 |
| Motor izquierdo, entrada 2 | D6 (PWM) | DRV8833 AIN2 |
| Motor derecho, entrada 1 | D9 (PWM) | DRV8833 BIN1 |
| Motor derecho, entrada 2 | D10 (PWM) | DRV8833 BIN2 |
| Habilitar driver | 5 V (o un pin libre) | DRV8833 nSLEEP / EEP |
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

Quedan libres A6, A7 y D11.

## Salidas del DRV8833

| Salida | Va a |
|---|---|
| AOUT1, AOUT2 | Motor izquierdo |
| BOUT1, BOUT2 | Motor derecho |

Si un motor gira al revés, invierte sus dos cables (o invierte el sentido en el código).

## VL53L0X: direcciones I2C

Los tres arrancan con la dirección 0x29. Al iniciar el programa:

1. Poner D7, D8 y D12 en LOW (los tres sensores apagados).
2. Poner D7 en HIGH y asignar al sensor la dirección 0x30.
3. Poner D8 en HIGH y asignar 0x31.
4. Poner D12 en HIGH y asignar 0x32.

La librería `VL53L0X` de Pololu tiene `setAddress()` para esto.
