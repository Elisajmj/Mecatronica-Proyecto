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

byte rowPins[ROWS] = {30, 32, 34, 46};    // Keypad rows pins
byte colPins[COLS] = {31, 33, 35, 37};  

const char* codes[] = {"A11", "A12", "B11", "B12"};  // Codigos de los alimentos de la maquina
float prices[4] = {1.2, 2.0, 1.5, 1.0};  // Codigos de los alimentos de la maquina

const int numCodes = sizeof(codes) / sizeof(codes[0]); // Número de códigos

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int RS_PIN = 12, EN_PIN = 11, D4_PIN = 50, D5_PIN = 48, D6_PIN = 46, D7_PIN = 44;  // LCD Pins
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

void displayText (const char* string1, const char* string2) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(string1);

  lcd.setCursor(0, 1);
  lcd.print(string2);
}


void valid(bool isValid) {
  // si es valido, confirmamos compra, sino es valido
  // volvemos a pedir codigo
  if (isValid) {
    float price = select(enteredCode);
    displayText("Precio: ", " ");
    lcd.print(price);
    
    delay(1000);
    displayText("Pulse # para", "confirmar: ");
    
    isConfirmed = true; 
  } else {
    lcd.clear();
    lcd.noAutoscroll();
    displayText("Introduce codigo", " valido: ");

    memset(enteredCode, '\0', sizeof(enteredCode)); // Limpiar buffer
    data_count = 0; 
  }
}

void processCode(char key) {

  // Mientras se introduzcan menos de 3 digitos, cogemos
  // lo que nos pasan
	if (data_count < NUM_DIGITS) {
        enteredCode[data_count] = key;
        displayText("Codigo: ", enteredCode);
        delay(500);
        data_count++;
      }
  
  // Si tiene 3 digitos, comprobamos codigo
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
    
    valid(isValid);
  }
 } 


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  displayText("Introduzca", "   codigo: ");
}


void loop() {

  char key = keypad.getKey();
  
  if (key) {
    Serial.print(key);   

    if (!isConfirmed) { 
      processCode(key);
    
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
        displayText("Pulse # para", "confirmar: ");
      }
    }
  }
}