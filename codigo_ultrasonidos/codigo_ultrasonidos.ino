// PINS
const int TRIGGERPIN = 9;
const int ECHOPIN = 10;
const int GREENLED = 8;

const int DETECTCLIENT = 100; // min distance to detect new client
const int LECTORCARD = 3.00; // min card lector distance

int restart = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Init pins
  pinMode(TRIGGERPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(GREENLED, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (restart == 0) {
    // Start Vending Machine
    // digitalWrite(GREENLED, LOW);
    analogWrite(GREENLED, 0);
    delay(10000); // wait 10 sec

    if (getDistance() < DETECTCLIENT) {
      restart = 1;
      // Code for selecting product
    } else {
      waitForClient();
    }
  }

  if (restart == 1) {
    if (getDistance() <= LECTORCARD) {
      // digitalWrite(GREENLED, HIGH);
      analogWrite(GREENLED, 50);
      delay(3000);
      restart = 0; // reset to start program again
    } else {
      waitForPayment();
    }
  }

}

float getDistance() {
  long time;
  float distance;

  // Set trigger to low and wait 2 microsec
  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(2);

  // Send a 10 microsec pulse to trigger
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGERPIN, LOW);

  // Read time until echo receives something
  time = pulseIn(ECHOPIN, HIGH);

  // Calculate distance in cm
  // Sound speed = 34300 cm/s
  // distance = (time * 0.0343) / 2;
  distance = time / 29 / 2.0;

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

void waitForClient() {
  // LCD code
}

void waitForPayment() {
  // LCD code
}
