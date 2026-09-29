# Materiales

## Lo que tenemos

| Pieza | Cant. | ¿Sirve? | Notas |
|---|---|---|---|
| Motor 12 V 300 rpm | 2 | Depende del tamaño | Con N20 (≈10 g c/u) sí. Los de 25 o 37 mm pesan 80–200 g c/u y casi no caben en 10 × 10 cm. Con la LiPo 2S reciben 7.4–8.4 V: ≈185–210 rpm y ≈60–70 % del torque nominal. |
| Arduino Nano | 1 | Sí | Alcanzan los pines ([conexiones](conexiones.md)). |
| Batería LiPo 2S 7.4 V | 1 | Sí | Falta confirmar la capacidad (lo recomendado es 400–600 mAh). |
| Llantas de 34 mm | 2 | Sí | Con ≈200 rpm dan ≈0.36 m/s. |
| DRV8833 | 2 | Sí | Hasta **10.8 V**: va con 2S, **nunca con 3S**. ≈1.5 A continuos por canal. Una placa mueve los 2 motores; la otra es el repuesto. |
| VL53L0X | 3 | Sí | Sensores de oponente al frente (izquierda, centro, derecha). Hay que usar el pin XSHUT de cada uno para darles direcciones I2C distintas. |
| TCRT5000 | 4 | Sí | Sensores de borde, uno en cada esquina, a 2–5 mm del piso. |
| MPU6050 | 2 | No para la v1 | El reglamento no los pide. |
| Switch | 1 | Sí | Para prender y apagar (on/off). |

## Lo que falta

| Pieza | Cant. | Por qué |
|---|---|---|
| Regulador buck a 5 V | 1 | Para que el Nano no se reinicie cuando los motores frenan o invierten |
| Pulsador de inicio | 1 | Arranque reglamentario de 5 s |
| DIP switch o 2 pulsadores (opcional) | 1 | Elegir estrategia antes del combate |
| Chasis propio (PLA+ o PETG) | 1 | Obligatorio por reglamento |
| Pala metálica fija de 95–98 mm | 1 | Meterse debajo del rival y sumar peso abajo |
| Cargador balanceador 2S | 1 | Obligatorio por las bases |
| Bolsa ignífuga para LiPo | 1 | Obligatoria por las bases |
| Cables, conectores y tornillos | — | — |

## Repuestos sugeridos

1 motor, 1 LiPo 2S, 1–2 TCRT5000, 1 VL53L0X y el segundo DRV8833.

## Presupuesto de peso (si los motores son N20)

| Pieza | g aprox. |
|---|---|
| 2 motores N20 + soportes | 25 |
| 2 llantas de 34 mm | 15 |
| LiPo 2S 500 mAh | 35 |
| Nano + DRV8833 + buck | 15 |
| 3 VL53L0X + 4 TCRT5000 | 25 |
| Cables, switch y tornillos | 30 |
| Chasis impreso | 70–100 |
| **Subtotal** | **≈215–245** |
| Pala de acero + lastre bajo | hasta llegar a ≈480 |

El empuje máximo antes de patinar es F ≈ μ·m·g, así que conviene llegar a ≈480 g con el peso lo más abajo posible.
