#include <Arduino.h>
#include "ESP32Servo.h"
#include <AccelStepper.h>

#define IN4 2
#define IN3 3
#define IN2 4
#define IN1 5
#define Servo_PIN 6
#define Final_Carrera 7
#define Motor_DC_1 8
#define Motor_DC_2 9
#define STEP1 10
#define DIR1 11
#define STEP2 12
#define DIR2 13

#define Steps_cajon 6000
void CargarCatapulta()
{
  digitalWrite(Motor_DC_2, LOW);
}

void DescargarCatapulta()
{
  digitalWrite(Motor_DC_1, LOW);
}
void PararCargaCatapulta()
{
  digitalWrite(Motor_DC_2, HIGH);
  digitalWrite(Motor_DC_1, HIGH);
}
void SacarDisco();
void MeterDisco();
void SacarCajon();
void MeterCajon();

Servo miServo;
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
void setup()
{

  // Setup NEMA17
  pinMode(STEP1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(Motor_DC_1, OUTPUT);
  pinMode(Motor_DC_2, OUTPUT);

  pinMode(Final_Carrera, INPUT_PULLUP);

  digitalWrite(Motor_DC_1, HIGH);
  digitalWrite(Motor_DC_2, HIGH);
  miServo.attach(Servo_PIN);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(1000);
  stepper.setSpeed(500);
}
void loop()
{
  // DescargarCatapulta();
  // delay(1000);
  // PararCargaCatapulta();
  // delay(10000);
  SacarCajon();
  miServo.write(0);
  delay(1000);
  while (digitalRead(Final_Carrera) == HIGH)
    CargarCatapulta();
  delay(150);
  PararCargaCatapulta();
  miServo.write(65);
  delay(1000);
  DescargarCatapulta();
  delay(10000);
  PararCargaCatapulta();
  SacarDisco();
  delay(1000);
  MeterDisco();
  delay(1000);
  MeterCajon();
  delay(100000);
  // if (digitalRead(10) == LOW)
  // {
  //   miServo.write(0); // Mover el servo a 90 grados
  //   delay(1000);
  //   while (digitalRead(9) == HIGH)
  //   {
  //     digitalWrite(7, LOW);
  //     digitalWrite(8, HIGH);
  //   }
  //   delay(150);
  //   digitalWrite(7, HIGH);
  //   digitalWrite(8, HIGH);
  //   miServo.write(65); // Mover el servo a 0 grados
  //   delay(1000);
  //   digitalWrite(7, HIGH);
  //   digitalWrite(8, LOW);
  //   delay(10000);
  //   digitalWrite(7, HIGH);
  //   digitalWrite(8, HIGH);
  //   stepper.moveTo(2 * 2048);
  //   while (stepper.distanceToGo() != 0)
  //     stepper.run();
  //   delay(1000); // Esperar 1 segundo
  //   stepper.moveTo(0);
  //   while (stepper.distanceToGo() != 0)
  //     stepper.run();
  //   delay(1000); // Esperar 1 segundo
  // }
  // miServo.write(65); // Mover el servo a 90 grados
  // delay(100);
}

void SacarCajon()
{
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);

  for (int i = 0; i < Steps_cajon; i++)
  {
    digitalWrite(STEP1, HIGH);
    digitalWrite(STEP2, HIGH);
    delayMicroseconds(250);
    digitalWrite(STEP1, LOW);
    digitalWrite(STEP2, LOW);
    delayMicroseconds(250);
  }
}

void MeterCajon()
{
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);

  for (int i = 0; i < Steps_cajon; i++)
  {
    digitalWrite(STEP1, HIGH);
    digitalWrite(STEP2, HIGH);
    delayMicroseconds(250);
    digitalWrite(STEP1, LOW);
    digitalWrite(STEP2, LOW);
    delayMicroseconds(250);
  }
}

void SacarDisco()
{
  stepper.moveTo(2 * 2048);
  while (stepper.distanceToGo() != 0)
    stepper.run();
}

void MeterDisco()
{
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0)
    stepper.run();
}