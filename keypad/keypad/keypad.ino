#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4; // Filas keypad 
const byte COLS = 4; // Filas keypad
const byte NUM_DIGITS = 3; // 3 digitos para elegir comida
const byte LCD_COLS = 16; // Columnas del LCD
const byte LCD_ROWS = 2; // Filas del LCD

int data_count = 0;
char enteredCode[NUM_DIGITS + 1];

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {53, 51, 49, 47}; // Pines de las filas del keypad
byte colPins[COLS] = {45, 43, 41, 39};  // Pines de las columnas del keypad

const char* codes[] = {"A11", "A12", "B11", "B12"};  // Codigos de los alimentos de la maquina
float prices[4] = {1.2, 2.0, 1.5, 1.0};  // Codigos de los alimentos de la maquina

const int numCodes = sizeof(codes) / sizeof(codes[0]); // Número de códigos

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int RS_PIN = 12, EN_PIN = 11, D4_PIN = 5, D5_PIN = 4, D6_PIN = 3, D7_PIN = 2;  //Pines utilizados del LCD
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

bool isConfirmed = false; 
bool isValid = false;

float select(char enteredCode[NUM_DIGITS + 1]) {
  for (int i = 0; i < numCodes; i++) {
      if (strcmp(enteredCode, codes[i]) == 0) {
        return prices[i];
      }
    }
  
  return 0;
}

void display_text (const char* string1, const char* string2) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(string1);

  lcd.setCursor(0, 1);
  lcd.print(string2);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  display_text("Introduzca", "   codigo: ");
}

void loop() {

  char key = keypad.getKey();
  
  if (key) {
    Serial.print(key);   
    

    if (!isConfirmed) { 
      
      if (data_count < NUM_DIGITS) {
        enteredCode[data_count] = key;
        display_text("Codigo: ", enteredCode);
        delay(500);
        data_count++;
      }

      if (data_count == NUM_DIGITS) {
        enteredCode[data_count] = '\0'; // Agregar terminador nulo
        Serial.print("\nCódigo introducido: ");
        Serial.println(enteredCode);
      
        isValid = false;
        for (int i = 0; i < numCodes; i++) {
          if (strcmp(enteredCode, codes[i]) == 0) {
            isValid = true;
            break;
          }
        }

        if (isValid) {

          float price = select(enteredCode);
          display_text("Precio: ", " ");
          lcd.print(price);
          
          delay(1000);
        
          display_text("Pulse # para", "confirmar: ");
          
          isConfirmed = true; 
        } else {

          lcd.clear();
          lcd.noAutoscroll();
    
          display_text("Introduce codigo", " valido: ");
    
          memset(enteredCode, '\0', sizeof(enteredCode)); // Limpiar buffer
          data_count = 0; 
        }
      }

    } else { 
      if (key == '#') {
        lcd.clear();
        
        lcd.setCursor(16, 0);
        lcd.autoscroll();
        const char* confirm = "Compra confirmada.          Acerque tarjeta ";

        for (int i = 0; i < strlen(confirm); i++) {
          lcd.print(confirm[i]); 
          delay(200);            
        }

        //Pagar aquí, tener en cuenta el ultrasonido
        isConfirmed = false; 
        memset(enteredCode, '\0', sizeof(enteredCode)); 
        data_count = 0; 
      } else {

        lcd.noAutoscroll();
        display_text("Pulse # para", "confirmar: ");
      }
    }
  }
}