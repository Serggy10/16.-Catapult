const int stepPin = 9;
const int dirPin = 10;
const int dirPin2 = 12;
const int stepPin2 = 11;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
}

void loop() {
  digitalWrite(dirPin, HIGH);
  digitalWrite(dirPin2, LOW);

  for (int i = 0; i < 6000; i++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(250);
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(250);
  }

  delay(1000);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin, LOW);

  for (int i = 0; i < 6000; i++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(250);
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(250);
  }

  delay(1000);
}