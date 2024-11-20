#include <Servo.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// First we define the sates of the vending machine
enum State {
  WAITING_CLIENT,   // Waiting for a new customer
  TAKING_ORDER,     // Reciving data of the order
  PROCESSING_SALE,  // Processing the given order
  IDLE_STATE        // Idle state to let the customer get the order
};

// General variables
const int ORDER_LEN = 3;    // Order format "A11"
const byte ROWS = 4;        // Rows of keypad 
const byte COLS = 4;        // Columns of keypad
const char* codes[] = {"A11", "A12",    // Codes of the different foods
                    "B11", "B12"};      // in order as shown A11 up right, A12 up left...

State current_state = WAITING_CLIENT;
bool customerPresent = false;
char selectedProduct[ORDER_LEN+1];    // max string format 3 "A11\0"
int len_order = 0;  // used to follow the keypad order
char new_key;


// Servos variables
Servo servos[4];                    // array with the 4 servos
int pinServos[] = {2, 3, 4, 5};     // Pins
int numServos = 4;
int rotTime = 1200;                 // time of a rotation in ms

// Keypad variables
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {30, 32, 34, 46};    // Keypad rows pins
byte colPins[COLS] = {31, 33, 35, 37};    // Keypad columns pins
float prices[] = {1.2, 2.0, 1.5, 1.0};    // Prices of the food
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD variables
const int RS_PIN = 12, EN_PIN = 11, D4_PIN = 5, D5_PIN = 4, D6_PIN = 3, D7_PIN = 2;  // LCD Pins
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

// Ultrasound data
int button_pin = 20;        // esto es provisional porque no tengo el ultrasonidos
long lastInterruptTime;     // esto es provisional porque no tengo el ultrasonidos
long debounceDelay = 200;
bool client_near = false;

// This functionevalates if the code is correct
bool code_valid(char* code) {
  int codes_len = sizeof(codes) / sizeof(codes[0]);

  for(int i=0; i<codes_len; i++){
    if(!strcmp(codes[i], code))
      return true;
  }

  // if code not finded in valid codes
  return false;
}

void setup() {
  Serial.println("Starting vending machine...");
  // Servos initialization
  for (int i = 0; i < numServos; i++) {
    servos[i].attach(pinServos[i]); 
  }

  // Ultrasound initialization
  pinMode(button_pin, INPUT_PULLUP);  // esto es provisional porque no tengo el ultrasonidos
  attachInterrupt(digitalPinToInterrupt(button_pin), handleButtonPress, RISING);

  strcpy(selectedProduct, "");    // initialize the product string

  Serial.begin(9600);
  Serial.println("Vending machine set up");
}

// esto es provisional porque no tengo el ultrasonidos
void handleButtonPress() {
  unsigned long currentMillis = millis();
   // Verificar si ha pasado suficiente tiempo desde el último cambio
  if (currentMillis - lastInterruptTime > debounceDelay) {
    lastInterruptTime = currentMillis; // Actualizamos el tiempo del último interrupt
    Serial.print("Boton pulsao cliente cerca: ");
    client_near = !client_near;
    Serial.println(client_near);
  }
}

void loop() {
  switch(current_state){
    case WAITING_CLIENT:    // Waiting for a new customer
    // led verde encendido

    // If the ultrasound sets that a client is near, we pass state
    if(client_near){
      current_state = TAKING_ORDER;
      Serial.println("Client near passing to TAKING_ORDER");
    }
    break;

    case TAKING_ORDER:      // Reciving data of the order
    // Read the pad until full order is writen
    if((new_key = keypad.getKey()) != NO_KEY){
      selectedProduct[len_order] = new_key;
      Serial.print("Nueva key: ");
      Serial.println(selectedProduct);
      len_order++;
    }

    // we get a full order
    if(len_order == ORDER_LEN){
      len_order = 0;

      // if is valid we get to next state
      if(code_valid(selectedProduct)){
        Serial.print("Got order ");
        Serial.print(selectedProduct);
        Serial.println(" passing to PROCESSING_SALE");
        current_state = PROCESSING_SALE;
      } else {
        Serial.print(selectedProduct);
        Serial.println(" wrong order");
        memset(selectedProduct, 0, sizeof(selectedProduct));
      }
    } 
    // If given order is valid get to next state

    // else if the client walks away we restar the order and go back to waiting client
    if(!client_near){
      current_state = WAITING_CLIENT;
      len_order = 0;
      memset(selectedProduct, 0, sizeof(selectedProduct));
      Serial.println("Client walks away! pass to WAITING_CLIENT");
    }
    // else restart order
    break;

    case PROCESSING_SALE:   // Processing the given order
    // set the servomotor of product to run
    // if the server is finished we send that is ok and continue
    break;
    case IDLE_STATE:
    // wait until client gets the product
    break;
  }

  delay(100);   // avoid loop saturation
}
