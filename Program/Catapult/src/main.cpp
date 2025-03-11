#include <Arduino.h>
#include "ESP32Servo.h"
#include <AccelStepper.h>
#include <WiFi.h>
#include <SinricPro.h>
#include "Accionador.h"

#define APP_KEY "f5b6b057-f65b-4c34-b595-489667228191"
#define APP_SECRET "d9760d34-c170-4323-9a23-87303b92d7ba-b098499e-3931-4297-93c6-d295e4dc6d9d"
#define DEVICE_ID "67cf53f26066f22be0642e65"

#define SSID "DIGIFIBRA-ctUQ"
#define PASS "3T67UeAPkNU7"

#define BAUD_RATE 115200

#define ENABLE 1
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

#define Steps_cajon 7000
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
void TareaSinricPro(void *Pvparameters);
void Control(void *Pvparameters);

Servo miServo;
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
Accionador &accionador = SinricPro[DEVICE_ID];
bool activado = false;
bool abrir = false;
bool cerrar = false;
bool cajonAbierto = false;

/*************
 * Variables *
 ***********************************************
 * Global variables to store the device states *
 ***********************************************/

// ToggleController
std::map<String, bool> globalToggleStates;
// ModeController
std::map<String, String> globalModes;

/*************
 * Callbacks *
 *************/

// ToggleController
bool onToggleState(const String &deviceId, const String &instance, bool &state)
{
  Serial.printf("[Device: %s]: State for \"%s\" set to %s\r\n", deviceId.c_str(), instance.c_str(), state ? "on" : "off");
  globalToggleStates[instance] = state;
  activado = state;
  return true;
}

// ModeController
bool onSetMode(const String &deviceId, const String &instance, String &mode)
{
  Serial.printf("[Device: %s]: Modesetting for \"%s\" set to mode %s\r\n", deviceId.c_str(), instance.c_str(), mode.c_str());
  globalModes[instance] = mode;
  if (mode == "activado" && cajonAbierto == false)
    abrir = true;
  else if (mode == "desactivado" && cajonAbierto == true)
    cerrar = true;
  return true;
}

/**********
 * Events *
 *************************************************
 * Examples how to update the server status when *
 * you physically interact with your device or a *
 * sensor reading changes.                       *
 *************************************************/

// ToggleController
void updateToggleState(String instance, bool state)
{
  Serial.printf("[Update state for \"%s\" set to %s\r\n", instance.c_str(), state ? "on" : "off");
  accionador.sendToggleStateEvent(instance, state);
}

// ModeController
void updateMode(String instance, String mode)
{
  accionador.sendModeEvent(instance, mode, "PHYSICAL_INTERACTION");
}

/*********
 * Setup *
 *********/

void setupSinricPro()
{

  // ToggleController
  accionador.onToggleState("toggleInstance1", onToggleState);
  // ModeController
  accionador.onSetMode("modeInstance1", onSetMode);
  SinricPro.onConnected([]
                        { Serial.printf("[SinricPro]: Connected\r\n"); });
  SinricPro.onDisconnected([]
                           { Serial.printf("[SinricPro]: Disconnected\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
};

void setupWiFi()
{
#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
#elif defined(ESP32)
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
#endif

  WiFi.begin(SSID, PASS);
  delay(1000);
  Serial.printf("[WiFi]: Connecting to %s", SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected\r\n");
}

void setup()
{
  pinMode(ENABLE, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(Motor_DC_1, OUTPUT);
  pinMode(Motor_DC_2, OUTPUT);
  pinMode(Final_Carrera, INPUT_PULLUP);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(BAUD_RATE);
  setupWiFi();
  setupSinricPro();

  xTaskCreatePinnedToCore(
      TareaSinricPro,   // Función de la tarea
      "TareaSinricPro", // Nombre de la tarea
      10000,            // Tamaño de la pila
      NULL,             // Parámetros de entrada
      1,                // Prioridad de la tarea
      NULL,             // Manejador de la tarea (puede ser NULL si no se necesita)
      0);               // Núcleo en el que se ejecutará la tarea (1 para el segundo núcleo)
  xTaskCreatePinnedToCore(
      Control,        // Función de la tarea
      "TareaControl", // Nombre de la tarea
      10000,          // Tamaño de la pila
      NULL,           // Parámetros de entrada
      1,              // Prioridad de la tarea
      NULL,           // Manejador de la tarea (puede ser NULL si no se necesita)
      1);             // Núcleo en el que se ejecutará la tarea (1 para el segundo núcleo)
}
void loop()
{
  vTaskDelay(10 / portMAX_DELAY);
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

void TareaSinricPro(void *Pvparameters)
{
  for (;;)
  {
    SinricPro.handle();
    delay(10);
  }
}
void Control(void *Pvparameters)
{
  digitalWrite(Motor_DC_1, HIGH);
  digitalWrite(Motor_DC_2, HIGH);
  miServo.attach(Servo_PIN);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(1000);
  stepper.setSpeed(500);
  for (;;)
  {
    if (activado)
    {
      SacarCajon();
      miServo.write(0); // Lanzar carga
      delay(1000);
      while (digitalRead(Final_Carrera) == HIGH)
        CargarCatapulta();
      delay(150);
      PararCargaCatapulta();
      miServo.write(65); // Trabar catapulta
      delay(1000);
      DescargarCatapulta();
      delay(10000); // Tiempo para soltar el hilo que recoge la catapulta
      PararCargaCatapulta();
      MeterCajon();
      delay(1000);
      SacarDisco();
      delay(1000);
      MeterDisco();
      activado = false;
      updateToggleState("toggleInstance1", activado);
    }
    if (abrir)
    {
      SacarCajon();
      cajonAbierto = true;
      abrir = false;
    }
    if (cerrar)
    {
      MeterCajon();
      cajonAbierto = false;
      cerrar = false;
    }
    vTaskDelay(100 / portMAX_DELAY);
  }
}