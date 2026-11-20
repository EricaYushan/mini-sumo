/*
  Prueba de motores: Arduino Nano + DRV8833 + LiPo 2S

  ANTES DE SUBIRLO
  - Robot con las ruedas EN EL AIRE (sobre una caja o un vaso).
  - Nano conectado por USB. La batería solo alimenta al DRV8833 (pin VCC).
  - GND de la batería, del DRV8833 y del Nano unidos.
  Cableado completo: firmware/pruebas/README.md

  USO
  Abre el Monitor Serie a 115200 baudios, con "Nueva línea" o "Sin ajuste de
  línea", y escribe una letra + Enter:

    w  adelante          s  atrás
    a  girar izquierda   d  girar derecha (sobre su eje)
    i  solo motor izq.   o  solo motor der. (adelante)
    x  soltar (coast)    b  frenar (brake)
    1..9  velocidad (1 = 11 %, 9 = 100 %). Arranca en 4 (≈44 %).
    p  rutina automática: adelante, atrás, giros, frenado
    f  ver el estado del pin ULT (falla), si lo conectaste a D11

  QUÉ REVISAR
  - "w" debe mover las DOS ruedas hacia adelante. Si una gira al revés,
    cambia INVERTIR_IZQ o INVERTIR_DER abajo (o invierte sus dos cables).
  - "a" debe girar el robot a la izquierda. Si gira a la derecha, están
    cruzados los motores: el izquierdo va en OUT1/OUT2 (D5/D6).
  - Cuando la placa arranca imprime "=== INICIO ===". Si lo vuelves a ver
    mientras los motores se mueven, el Nano se REINICIÓ por una caída de
    voltaje. Ver la sección "Si el Nano se reinicia" del README.
*/

// ---------- Pines (hardware/conexiones.md) ----------
const int IZQ_1 = 5;   // DRV8833 IN1 (AIN1)
const int IZQ_2 = 6;   // DRV8833 IN2 (AIN2)
const int DER_1 = 9;   // DRV8833 IN3 (BIN1)
const int DER_2 = 10;  // DRV8833 IN4 (BIN2)
const int PIN_FALLA = 11;  // DRV8833 ULT (nFAULT), opcional

// Cambia a true si un motor gira al revés con "w"
const bool INVERTIR_IZQ = false;
const bool INVERTIR_DER = false;

// Pausa al cambiar de sentido. Evita el pico de corriente de pasar de
// adelante a atrás de golpe, que es lo que más reinicia al Nano.
const unsigned long PAUSA_INVERSION_MS = 120;

int velocidad = 113;   // 0..255 (nivel 4)
int ultimoIzq = 0, ultimoDer = 0;

// Un motor: v de -255 (atrás) a 255 (adelante), 0 = suelto.
// DRV8833 en "fast decay": una entrada con PWM y la otra en LOW.
void motor(int pin1, int pin2, int v, bool invertir) {
  if (invertir) v = -v;
  v = constrain(v, -255, 255);
  if (v > 0) {
    digitalWrite(pin2, LOW);
    analogWrite(pin1, v);
  } else if (v < 0) {
    digitalWrite(pin1, LOW);
    analogWrite(pin2, -v);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

void motores(int izq, int der) {
  // Si alguno cambia de sentido, soltar primero y esperar
  bool inversion = ((long)izq * ultimoIzq < 0) || ((long)der * ultimoDer < 0);
  if (inversion) {
    motor(IZQ_1, IZQ_2, 0, false);
    motor(DER_1, DER_2, 0, false);
    delay(PAUSA_INVERSION_MS);
  }
  motor(IZQ_1, IZQ_2, izq, INVERTIR_IZQ);
  motor(DER_1, DER_2, der, INVERTIR_DER);
  ultimoIzq = izq;
  ultimoDer = der;
}

// Freno activo: las dos entradas en HIGH (el motor queda en corto y se
// detiene rápido). Usa más corriente que soltar.
void frenar() {
  digitalWrite(IZQ_1, HIGH); digitalWrite(IZQ_2, HIGH);
  digitalWrite(DER_1, HIGH); digitalWrite(DER_2, HIGH);
  ultimoIzq = ultimoDer = 0;
}

void soltar() { motores(0, 0); }

void revisarFalla() {
  // ULT es de drenador abierto: LOW = el DRV8833 se apagó por sobrecorriente,
  // sobretemperatura o bajo voltaje.
  if (digitalRead(PIN_FALLA) == LOW) {
    Serial.println(F("  !! ULT en LOW: el DRV8833 reporta FALLA (corriente, temperatura o voltaje)"));
  }
}

void paso(const __FlashStringHelper *nombre, int izq, int der, unsigned long ms) {
  Serial.print(F("  ")); Serial.println(nombre);
  motores(izq, der);
  unsigned long t0 = millis();
  while (millis() - t0 < ms) { revisarFalla(); delay(50); }
}

void rutina() {
  Serial.println(F("Rutina automática (ruedas en el aire):"));
  int v = velocidad;
  paso(F("adelante"), v, v, 1500);
  paso(F("atrás"), -v, -v, 1500);
  paso(F("girar izquierda"), -v, v, 1000);
  paso(F("girar derecha"), v, -v, 1000);
  Serial.println(F("  rampa de 0 a 100 %"));
  for (int s = 0; s <= 255; s += 5) { motores(s, s); delay(30); }
  delay(800);
  Serial.println(F("  frenar"));
  frenar();
  delay(800);
  soltar();
  Serial.println(F("Fin de la rutina."));
}

void setup() {
  // Motores apagados lo antes posible
  pinMode(IZQ_1, OUTPUT); pinMode(IZQ_2, OUTPUT);
  pinMode(DER_1, OUTPUT); pinMode(DER_2, OUTPUT);
  soltar();
  pinMode(PIN_FALLA, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("=== INICIO ==="));
  Serial.println(F("Prueba de motores. Comandos: w s a d i o x b 1-9 p f"));
  Serial.print(F("Velocidad: ")); Serial.println(velocidad);
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == '\n' || c == '\r' || c == ' ') return;

  digitalWrite(LED_BUILTIN, HIGH);
  int v = velocidad;
  switch (c) {
    case 'w': Serial.println(F("adelante")); motores(v, v); break;
    case 's': Serial.println(F("atrás")); motores(-v, -v); break;
    case 'a': Serial.println(F("girar izquierda")); motores(-v, v); break;
    case 'd': Serial.println(F("girar derecha")); motores(v, -v); break;
    case 'i': Serial.println(F("solo motor izquierdo")); motores(v, 0); break;
    case 'o': Serial.println(F("solo motor derecho")); motores(0, v); break;
    case 'x': Serial.println(F("soltar")); soltar(); break;
    case 'b': Serial.println(F("frenar")); frenar(); break;
    case 'p': rutina(); break;
    case 'f':
      Serial.print(F("ULT (D11) = "));
      Serial.println(digitalRead(PIN_FALLA) == LOW ? F("LOW: FALLA") : F("HIGH: sin falla"));
      break;
    default:
      if (c >= '1' && c <= '9') {
        velocidad = (c - '0') * 255 / 9;
        Serial.print(F("Velocidad: ")); Serial.print(velocidad);
        Serial.print(F(" (")); Serial.print(velocidad * 100 / 255); Serial.println(F(" %)"));
        // Aplicar la nueva velocidad si ya se estaba moviendo
        if (ultimoIzq != 0 || ultimoDer != 0) {
          int si = (ultimoIzq > 0) - (ultimoIzq < 0);
          int sd = (ultimoDer > 0) - (ultimoDer < 0);
          motores(si * velocidad, sd * velocidad);
        }
      } else {
        Serial.print(F("Comando desconocido: ")); Serial.println(c);
      }
  }
  revisarFalla();
  digitalWrite(LED_BUILTIN, LOW);
}
