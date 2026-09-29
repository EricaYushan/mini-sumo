/*
  MiniSumo: simulación en Tinkercad Circuits

  Tinkercad no tiene Arduino Nano, DRV8833, VL53L0X ni TCRT5000, así que en
  la simulación se reemplazan por:
    Arduino Nano -> Arduino Uno R3 (mismo microcontrolador y mismos pines)
    DRV8833      -> L293D (se controla igual: dos entradas por motor)
    VL53L0X x3   -> 3 sensores ultrasónicos HC-SR04
    TCRT5000 x4  -> 4 potenciómetros (girado a 0 = línea blanca)

  La máquina de estados es la misma que usará el robot real. Para pasar al
  robot real solo cambian leerRival() y leerBorde().
*/

// ---------- Pines ----------
// Motores (L293D en la simulación, DRV8833 en el robot real)
const int MOT_IZQ_1 = 5;   // L293D Input 1
const int MOT_IZQ_2 = 6;   // L293D Input 2
const int MOT_DER_1 = 9;   // L293D Input 3
const int MOT_DER_2 = 10;  // L293D Input 4

// Sensores de rival (HC-SR04 en la simulación): TRIG, ECHO
const int RIVAL_IZQ_TRIG = 7,  RIVAL_IZQ_ECHO = 8;
const int RIVAL_CEN_TRIG = 11, RIVAL_CEN_ECHO = 12;
const int RIVAL_DER_TRIG = A4, RIVAL_DER_ECHO = A5;

// Sensores de borde (potenciómetros en la simulación)
const int BORDE_FI = A0;  // frontal izquierdo
const int BORDE_FD = A1;  // frontal derecho
const int BORDE_TI = A2;  // trasero izquierdo
const int BORDE_TD = A3;  // trasero derecho

const int BOTON_INICIO = 2;
const int LED_ESTADO = 13;

// ---------- Ajustes ----------
const int UMBRAL_BLANCO = 300;      // lectura menor = línea blanca (calibrar en el dojo real)
const int UMBRAL_RIVAL_CM = 60;     // a menos de esto se considera que hay rival
const unsigned long ESPERA_INICIO_MS = 5000;  // reglamento: 5 s

const int VEL_BUSQUEDA = 150;
const int VEL_SEGUIR = 200;
const int VEL_ATAQUE = 255;
const int VEL_ESCAPE = 220;

// ---------- Estados ----------
enum Estado { WAIT_START, CUENTA_REGRESIVA, SEARCH, TRACK, ATTACK, EDGE_ESCAPE };
Estado estado = WAIT_START;
unsigned long tiempoEstado = 0;

// Escape de borde: 0 = retroceder, 1 = girar
int fasEscape = 0;
bool escapeHaciaAtras = true;   // true si el borde se vio adelante
int giroEscape = 1;             // 1 = girar a la derecha, -1 = a la izquierda

// Prototipos (Tinkercad no siempre los genera solo)
void motores(int izq, int der);
void cambiarEstado(Estado nuevo);

void setup() {
  Serial.begin(9600);

  pinMode(MOT_IZQ_1, OUTPUT);
  pinMode(MOT_IZQ_2, OUTPUT);
  pinMode(MOT_DER_1, OUTPUT);
  pinMode(MOT_DER_2, OUTPUT);

  pinMode(RIVAL_IZQ_TRIG, OUTPUT); pinMode(RIVAL_IZQ_ECHO, INPUT);
  pinMode(RIVAL_CEN_TRIG, OUTPUT); pinMode(RIVAL_CEN_ECHO, INPUT);
  pinMode(RIVAL_DER_TRIG, OUTPUT); pinMode(RIVAL_DER_ECHO, INPUT);

  pinMode(BOTON_INICIO, INPUT_PULLUP);
  pinMode(LED_ESTADO, OUTPUT);

  motores(0, 0);
  Serial.println("Listo. Presiona el boton de inicio.");
}

// ---------- Motores ----------
// velocidad de -255 (atrás) a 255 (adelante)
void motor(int pin1, int pin2, int velocidad) {
  velocidad = constrain(velocidad, -255, 255);
  if (velocidad >= 0) {
    analogWrite(pin1, velocidad);
    digitalWrite(pin2, LOW);
  } else {
    digitalWrite(pin1, LOW);
    analogWrite(pin2, -velocidad);
  }
}

void motores(int izq, int der) {
  motor(MOT_IZQ_1, MOT_IZQ_2, izq);
  motor(MOT_DER_1, MOT_DER_2, der);
}

// ---------- Sensores ----------
long distanciaCm(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long us = pulseIn(echo, HIGH, 6000);  // 6 ms ≈ 1 m como máximo
  if (us == 0) return 999;              // no hay nada
  return us / 58;
}

// En el robot real esto lee los VL53L0X.
void leerRival(bool &izq, bool &cen, bool &der) {
  izq = distanciaCm(RIVAL_IZQ_TRIG, RIVAL_IZQ_ECHO) < UMBRAL_RIVAL_CM;
  cen = distanciaCm(RIVAL_CEN_TRIG, RIVAL_CEN_ECHO) < UMBRAL_RIVAL_CM;
  der = distanciaCm(RIVAL_DER_TRIG, RIVAL_DER_ECHO) < UMBRAL_RIVAL_CM;
}

// En el robot real esto lee los TCRT5000 (misma lógica, otro umbral).
void leerBorde(bool &fi, bool &fd, bool &ti, bool &td) {
  fi = analogRead(BORDE_FI) < UMBRAL_BLANCO;
  fd = analogRead(BORDE_FD) < UMBRAL_BLANCO;
  ti = analogRead(BORDE_TI) < UMBRAL_BLANCO;
  td = analogRead(BORDE_TD) < UMBRAL_BLANCO;
}

void cambiarEstado(Estado nuevo) {
  if (nuevo == estado) return;
  estado = nuevo;
  tiempoEstado = millis();
  const char *nombres[] = {"WAIT_START", "CUENTA_REGRESIVA", "SEARCH", "TRACK", "ATTACK", "EDGE_ESCAPE"};
  Serial.print("Estado: ");
  Serial.println(nombres[nuevo]);
}

// ---------- Programa principal ----------
void loop() {
  unsigned long ahora = millis();

  if (estado == WAIT_START) {
    motores(0, 0);
    if (digitalRead(BOTON_INICIO) == LOW) cambiarEstado(CUENTA_REGRESIVA);
    return;
  }

  if (estado == CUENTA_REGRESIVA) {
    motores(0, 0);                                        // inmóvil por reglamento
    digitalWrite(LED_ESTADO, (ahora / 250) % 2);          // parpadea mientras espera
    if (ahora - tiempoEstado >= ESPERA_INICIO_MS) {
      digitalWrite(LED_ESTADO, HIGH);
      cambiarEstado(SEARCH);
    }
    return;
  }

  // 1) El borde tiene prioridad sobre todo lo demás
  bool fi, fd, ti, td;
  leerBorde(fi, fd, ti, td);
  if (estado != EDGE_ESCAPE && (fi || fd || ti || td)) {
    escapeHaciaAtras = (fi || fd);                  // borde adelante: retroceder
    giroEscape = (fi || ti) ? 1 : -1;               // borde a la izquierda: girar a la derecha
    fasEscape = 0;
    cambiarEstado(EDGE_ESCAPE);
  }

  if (estado == EDGE_ESCAPE) {
    unsigned long t = ahora - tiempoEstado;
    if (fasEscape == 0) {
      int v = escapeHaciaAtras ? -VEL_ESCAPE : VEL_ESCAPE;
      motores(v, v);
      if (t > 300) { fasEscape = 1; tiempoEstado = ahora; }
    } else {
      motores(VEL_ESCAPE * giroEscape, -VEL_ESCAPE * giroEscape);
      if (t > 250) cambiarEstado(SEARCH);
    }
    return;
  }

  // 2) Buscar y atacar
  bool rIzq, rCen, rDer;
  leerRival(rIzq, rCen, rDer);

  if (rCen && !rIzq && !rDer) {
    cambiarEstado(ATTACK);
    motores(VEL_ATAQUE, VEL_ATAQUE);
  } else if (rCen && rIzq && rDer) {
    cambiarEstado(ATTACK);                          // rival grande y de frente
    motores(VEL_ATAQUE, VEL_ATAQUE);
  } else if (rIzq) {
    cambiarEstado(TRACK);
    motores(VEL_SEGUIR / 3, VEL_SEGUIR);            // corregir hacia la izquierda
  } else if (rDer) {
    cambiarEstado(TRACK);
    motores(VEL_SEGUIR, VEL_SEGUIR / 3);            // corregir hacia la derecha
  } else {
    cambiarEstado(SEARCH);
    motores(VEL_BUSQUEDA, -VEL_BUSQUEDA);           // girar en el lugar
  }
}
