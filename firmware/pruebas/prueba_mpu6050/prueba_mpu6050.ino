/*
  Prueba del MPU6050 (módulo GY-521) en el Arduino Nano

  No necesita librerías: solo Wire, que viene con el IDE.
  Cableado: VCC -> 5 V, GND -> GND, SDA -> A4, SCL -> A5, AD0 -> GND o al aire.
  XDA, XCL e INT quedan sin conectar. Detalles en firmware/pruebas/README.md

  QUÉ HACE
  1. Busca todos los dispositivos del bus I2C y los lista. El MPU6050 debe
     salir en 0x68. Si ya conectaste los VL53L0X, salen en 0x29 (o en
     0x30-0x32 si otro programa ya les cambió la dirección y no se apagaron).
  2. Lee WHO_AM_I. Un MPU6050 original responde 0x68. Muchos clones
     responden 0x70, 0x72 o 0x98 (en realidad son MPU6500 u otro chip): para
     esta prueba también sirven.
  3. Calibra el giroscopio: DEJA EL ROBOT QUIETO los primeros 2 segundos.
  4. Imprime 10 veces por segundo:
       ax, ay, az  aceleración en g (quieto y plano: az ≈ 1.00, ax ≈ ay ≈ 0)
       gz          velocidad de giro en °/s alrededor del eje vertical
       giro        ángulo acumulado en grados (gira el robot 90° a mano y
                   debe marcar ≈ 90; a la izquierda suma, a la derecha resta)
       CHOQUE      si la aceleración horizontal pasa de UMBRAL_CHOQUE_G

  Abre el Monitor Serie a 115200 baudios. Escribe "c" + Enter para
  recalibrar y poner el ángulo en 0.
  El Serial Plotter también funciona: las columnas van como "nombre:valor".
*/

#include <Wire.h>

const uint8_t MPU = 0x68;   // AD0 en GND o al aire. Con AD0 en 3.3 V sería 0x69.

// Registros
const uint8_t PWR_MGMT_1   = 0x6B;
const uint8_t CONFIG       = 0x1A;
const uint8_t GYRO_CONFIG  = 0x1B;
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t ACCEL_XOUT_H = 0x3B;
const uint8_t WHO_AM_I     = 0x75;

// Escalas: acelerómetro ±8 g (los choques de sumo pasan de 2 g),
// giroscopio ±500 °/s (un giro rápido sobre su eje pasa de 250 °/s).
const float LSB_POR_G   = 4096.0;  // ±8 g
const float LSB_POR_DPS = 65.5;    // ±500 °/s

const float UMBRAL_CHOQUE_G = 1.5;  // ajustar con choques reales

float offsetGz = 0;
float anguloZ = 0;
unsigned long tAnterior = 0;
bool mpuOk = false;

bool escribir(uint8_t reg, uint8_t valor) {
  Wire.beginTransmission(MPU);
  Wire.write(reg);
  Wire.write(valor);
  return Wire.endTransmission() == 0;
}

bool leer(uint8_t reg, uint8_t *buf, uint8_t n) {
  Wire.beginTransmission(MPU);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU, n) != n) return false;
  for (uint8_t i = 0; i < n; i++) buf[i] = Wire.read();
  return true;
}

// ax, ay, az en g; gz en °/s (sin offset)
bool leerMPU(float &ax, float &ay, float &az, float &gz) {
  uint8_t b[14];
  if (!leer(ACCEL_XOUT_H, b, 14)) return false;
  int16_t rax = (b[0] << 8) | b[1];
  int16_t ray = (b[2] << 8) | b[3];
  int16_t raz = (b[4] << 8) | b[5];
  // b[6..7] temperatura, b[8..11] gx y gy (no se usan aquí)
  int16_t rgz = (b[12] << 8) | b[13];
  ax = rax / LSB_POR_G;
  ay = ray / LSB_POR_G;
  az = raz / LSB_POR_G;
  gz = rgz / LSB_POR_DPS;
  return true;
}

void escanearI2C() {
  Serial.println(F("Buscando dispositivos I2C..."));
  int encontrados = 0;
  for (uint8_t dir = 1; dir < 127; dir++) {
    Wire.beginTransmission(dir);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("  0x"));
      if (dir < 16) Serial.print('0');
      Serial.print(dir, HEX);
      if (dir == 0x68) Serial.print(F("  <- MPU6050"));
      else if (dir == 0x69) Serial.print(F("  <- MPU6050 con AD0 en alto"));
      else if (dir == 0x29) Serial.print(F("  <- VL53L0X (dirección de fábrica)"));
      else if (dir >= 0x30 && dir <= 0x32) Serial.print(F("  <- VL53L0X (dirección ya cambiada)"));
      Serial.println();
      encontrados++;
    }
  }
  if (encontrados == 0) {
    Serial.println(F("  Nada. Revisa VCC, GND, SDA->A4 y SCL->A5 (no los cruces)."));
  }
}

void calibrar() {
  Serial.println(F("Calibrando giroscopio: NO MUEVAS EL ROBOT (2 s)..."));
  float suma = 0;
  int n = 0;
  for (int i = 0; i < 400; i++) {
    float ax, ay, az, gz;
    if (leerMPU(ax, ay, az, gz)) { suma += gz; n++; }
    delay(5);
  }
  offsetGz = n ? suma / n : 0;
  anguloZ = 0;
  tAnterior = micros();
  Serial.print(F("Offset gz = ")); Serial.print(offsetGz, 2); Serial.println(F(" °/s"));
  if (fabs(offsetGz) > 10) {
    Serial.println(F("  Offset muy grande: ¿se movió durante la calibración? Escribe c para repetir."));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("=== Prueba MPU6050 ==="));

  Wire.begin();
  Wire.setClock(400000);
#if defined(WIRE_HAS_TIMEOUT)
  // Si el bus se traba (ruido de motores, cable flojo) no se cuelga el programa
  Wire.setWireTimeout(3000, true);
#endif

  escanearI2C();

  uint8_t id = 0;
  if (!leer(WHO_AM_I, &id, 1)) {
    Serial.println(F("No responde en 0x68. Revisa el cableado y que AD0 no esté en alto."));
    return;
  }
  Serial.print(F("WHO_AM_I = 0x")); Serial.print(id, HEX);
  if (id == 0x68) Serial.println(F("  (MPU6050)"));
  else Serial.println(F("  (clon o chip compatible; sigue la prueba)"));

  escribir(PWR_MGMT_1, 0x01);    // despertar, reloj del giroscopio X
  delay(100);
  escribir(CONFIG, 0x03);        // filtro pasa bajas ≈44 Hz: quita ruido de vibración
  escribir(GYRO_CONFIG, 0x08);   // ±500 °/s
  escribir(ACCEL_CONFIG, 0x10);  // ±8 g
  delay(50);

  mpuOk = true;
  calibrar();
}

void loop() {
  if (!mpuOk) return;

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'c') calibrar();
  }

  float ax, ay, az, gz;
  if (!leerMPU(ax, ay, az, gz)) {
    Serial.println(F("Error de lectura I2C"));
    delay(200);
    return;
  }

  // Integrar el giro (en cada vuelta, no solo al imprimir)
  unsigned long ahora = micros();
  float dt = (ahora - tAnterior) / 1e6;
  tAnterior = ahora;
  float gzCorr = gz - offsetGz;
  if (fabs(gzCorr) < 0.5) gzCorr = 0;   // zona muerta: menos deriva quieto
  anguloZ += gzCorr * dt;

  float horizontal = sqrt(ax * ax + ay * ay);
  static bool choque = false;
  if (horizontal > UMBRAL_CHOQUE_G) choque = true;

  static unsigned long tImpresion = 0;
  if (millis() - tImpresion >= 100) {
    tImpresion = millis();
    Serial.print(F("ax:")); Serial.print(ax, 2);
    Serial.print(F(" ay:")); Serial.print(ay, 2);
    Serial.print(F(" az:")); Serial.print(az, 2);
    Serial.print(F(" gz:")); Serial.print(gzCorr, 1);
    Serial.print(F(" giro:")); Serial.print(anguloZ, 1);
    if (choque) Serial.print(F("  CHOQUE"));
    Serial.println();
    choque = false;
  }
  delay(5);
}
