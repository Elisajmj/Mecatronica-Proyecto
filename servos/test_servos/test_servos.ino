#include <Servo.h>

Servo servos[4];                    // array with the 4 servos
int pinServos[] = {2, 3, 4, 5};     // Pins
int numServos = 4;
int rotTime = 1200;                 // in ms

void setup() {
  // Initialize each servo
  for (int i = 0; i < numServos; i++) {
    servos[i].attach(pinServos[i]); 
  }

  Serial.begin(9600); // Iniciar comunicación serial
  Serial.println("Prueba de 4 servos en rotación antihoraria");
}

void loop() {
  for (int i = 0; i < numServos; i++) {
    Serial.print("Probando servo ");
    Serial.println(i + 1);

    // Girar servo en dirección antihoraria
    servos[i].writeMicroseconds(2000); // Pulso para rotación antihoraria

    delay(rotTime); // Esperar el tiempo definido

    // Detener el servo
    servos[i].writeMicroseconds(1500); // Pulso para detener

    delay(1000); // Pausa antes de pasar al siguiente servo
  }
}
