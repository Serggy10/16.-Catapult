#include <Arduino.h>
#include "ESP32Servo.h"
#include <AccelStepper.h>

#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6
Servo miServo;
const int pinServo = 11; // Pin GPIO al que está conectado el servo
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
void setup()
{
  miServo.attach(pinServo);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(1000);
  stepper.setSpeed(500);
  pinMode(10, INPUT_PULLUP);
}
void loop()
{
  if (digitalRead(10) == LOW)
  {
    miServo.write(70); // Mover el servo a 0 grados
    stepper.moveTo(2 * 2048);
    while (stepper.distanceToGo() != 0)
      stepper.run();
    delay(1000); // Esperar 1 segundo
    stepper.moveTo(0);
    while (stepper.distanceToGo() != 0)
      stepper.run();
    delay(1000);      // Esperar 1 segundo
    miServo.write(0); // Mover el servo a 90 grados
    delay(1000);
  }
  miServo.write(0); // Mover el servo a 90 grados
  delay(100);
}