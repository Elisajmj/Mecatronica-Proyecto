#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const byte NUM_DIGITS = 3; // 3 digitos para elegir comida

int data_count = 0;
char enteredCode[NUM_DIGITS + 1];

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {53, 51, 49, 47};
byte colPins[COLS] = {45, 43, 41, 39};

const char* codes[] = {"A11", "A12", "B11", "B12"};  // Codigos de los alimentos de la maquina
const int numCodes = sizeof(codes) / sizeof(codes[0]); // Número de códigos


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.println("Iniciando...");
  Serial.begin(9600);

}

void loop() {
  char key = keypad.getKey();

  if (key) {
    
    Serial.println(key);

    if (data_count < NUM_DIGITS) {
      enteredCode[data_count] = key;
    }

    if (data_count == NUM_DIGITS) {

      Serial.print("Código introducido ");
      Serial.print(enteredCode);

      // chequeamos que el codigo introducido es valido
      bool isValid = false;
      
      for (int i = 0; i < numCodes; i++) {
        if (strcmp(enteredCode, codes[i]) == 0) {
          isValid = true;
          break;
        }
      }

      if (isValid) {
        Serial.println("\nPulse # para confirmar seleccion");
      } else {
        Serial.println("Introduzca código");
        memset(enteredCode, '\0', sizeof(enteredCode));
        data_count = 0;
        isValid = false;
      }


    }

    data_count++; 
  }

}

