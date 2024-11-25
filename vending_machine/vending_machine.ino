#include <Servo.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// First we define the sates of the vending machine
enum State {
  WAITING_CLIENT,   // Waiting for a new customer
  TAKING_ORDER,     // Reciving data of the order
  CHARGING,         // Charging the order
  PROCESSING_SALE,  // Processing the given order
  IDLE_STATE        // Idle state to let the customer get the order
};

// General variables
const int ORDER_LEN = 3;    // Order format "A11"
const byte ROWS = 4;        // Rows of keypad 
const byte COLS = 4;        // Columns of keypad
const int AWAY_CLIENT_TIME = 4000;      // Time to consider that the client is away in ms
const int TIME_TO_GET_PRODUCT = 2000;   // Time to let the client get the product in ms
const int TIME_SERVO_ROT = 2000;        // Time of rotation of the servos in ms
const int NEAR_CLIENT_DIST = 70;        // Distance threshold to set near client in cm
const int NEAR_CARD_DIST = 3;           // Distance threshold to charge a card in cm
const char* codes[] = {"A11", "A12",    // Codes of the different foods
                    "B11", "B12"};      // in order as shown A11 up right, A12 up left...

State current_state = WAITING_CLIENT;
bool customerPresent = false;
char selectedProduct[ORDER_LEN+1];    // max string format 3 "A11\0"
int len_order = 0;  // used to follow the keypad order
char new_key;

// Servos variables
Servo servos[4];                    // array with the 4 servos
int pinServos[] = {5, 4, 3, 2};     // Pins
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
const int trigger_pin = 8;
const int echo_pin = 9;
bool client_near = false;
int client_lost = 0;

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

// This function returns the distance of client to the ultrasound
float get_distance() {
  long time;
  float distance;

  // Set trigger to low and wait 2 microsec
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);

  // Send a 10 microsec pulse to trigger
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  // Read time until echo receives something
  time = pulseIn(echo_pin, HIGH);

  // Calculate distance in cm
  // Sound speed = 34300 cm/s
  // distance = (time * 0.0343) / 2;
  distance = time / 29 / 2.0;

  // Serial.print("Distancia: ");
  // Serial.print(distance);
  // Serial.println(" cm");

  return distance;
}

// This function returns a bool if the client is near true
// if the client goes away and is away for more than AWAY_CLIENT_TIME then return false
bool near_client(){
  int distance = get_distance();

  // if client isn't near and we detect it, if near return true, else false
  if(!client_near){
    client_near = distance <= NEAR_CLIENT_DIST;
  } else if (distance > NEAR_CLIENT_DIST){  // if we lost the client
    if(client_lost == 0){   // we just lost the client
      client_lost = millis(); // get the time of the lost
      client_near = true;
    } else {    // this isn't the first lost client loop
      if((millis() - client_lost) >= AWAY_CLIENT_TIME){ // if AWAY_CLIENT_TIME has passed since lost client
        client_lost = 0;  // restart the counter
        client_near = false;
      } else {    // no so long since we lost the client
        client_near = true;
      }
    }
  }

  return client_near;
}

// This function rotate the given servo ROTATE_TIME until the product is dropped
void rotate_servo(int servo){
  // Anti-hour rotation
  servos[servo].writeMicroseconds(2000);

  delay(TIME_SERVO_ROT); // wait until the rotation is made

  // Pulse to stop the servo
  servos[servo].writeMicroseconds(1500);
}

void setup() {
  Serial.println("Starting vending machine...");
  // Servos initialization
  for (int i = 0; i < numServos; i++) {
    servos[i].attach(pinServos[i]); 
  }

  // Ultrasound initialization
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  strcpy(selectedProduct, "");    // initialize the product string

  Serial.begin(9600);
  Serial.println("Vending machine set up");
}


void loop() {
  switch(current_state){
    case WAITING_CLIENT:    // Waiting for a new customer
      // If the ultrasound sets that a client is near, we pass state
      if(near_client()){
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
          current_state = CHARGING;
        } else {
          Serial.print(selectedProduct);
          Serial.println(" wrong order");
          memset(selectedProduct, 0, sizeof(selectedProduct));
        }
      } 
      // If given order is valid get to next state

      // else if the client walks away we restar the order and go back to waiting client
      if(!near_client()){
        current_state = WAITING_CLIENT;
        len_order = 0;
        memset(selectedProduct, 0, sizeof(selectedProduct));
        Serial.println("Client walks away! pass to WAITING_CLIENT");
      }
      // else restart order
      break;

    case CHARGING:
      // if the card is near the NFC (Ultrasound) then we "charge" and pass to next state
      if(get_distance() <= NEAR_CARD_DIST){
        current_state = PROCESSING_SALE;
      }

      // else if the client walks away we restar the order and go back to waiting client
      if(!near_client()){
        current_state = WAITING_CLIENT;
        len_order = 0;
        memset(selectedProduct, 0, sizeof(selectedProduct));
        Serial.println("Client walks away! pass to WAITING_CLIENT");
      }
      break;

    case PROCESSING_SALE:   // Processing the given order
      // Now we set the servomotor of the selected product to turn until the product is down
      if(!strcmp(selectedProduct, "A11"))
          rotate_servo(0);    // rotate the servo up right
      if(!strcmp(selectedProduct, "A12"))
          rotate_servo(1);    // rotate the servo up left
      if(!strcmp(selectedProduct, "B11"))
          rotate_servo(2);    // rotate the servo down right
      if(!strcmp(selectedProduct, "B12"))
          rotate_servo(3);    // rotate the servo down left
          
      // product has been processed we jump to idle state to let the client get the product
      current_state = IDLE_STATE;
      break;

    case IDLE_STATE:
      // wait until client gets the product
      delay(TIME_TO_GET_PRODUCT);
      // we restart the machine and get ready to new client
      len_order = 0;
      memset(selectedProduct, 0, sizeof(selectedProduct));
      Serial.println("Client walks away! pass to WAITING_CLIENT");
      current_state = WAITING_CLIENT;
      break;
  }

  delay(100);   // avoid loop saturation
}
