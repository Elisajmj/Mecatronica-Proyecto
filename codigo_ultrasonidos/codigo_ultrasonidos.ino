const int trigger_pin = 9;
const int echo_pin = 10;
const int green_led = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(green_led, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  float distance = getDistance();

  if (distance <= 3.00) {
    // digitalWrite(green_led, HIGH);
    analogWrite(green_led, 50);
    delay(3000);
  }
  // digitalWrite(green_led, LOW);
  analogWrite(green_led, 0);

  delay(500);

}

float getDistance() {
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

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}
