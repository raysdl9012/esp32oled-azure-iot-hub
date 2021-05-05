/**
 * Incluir librerias
 * */
#include <Arduino.h>
#include <WiFi.h>
#include "Esp32MQTTClient.h"

//****************************************
//******* DECLARACION DE VARIABLES *******
//****************************************

/**
 * Definir variables 
 * */
#define INPUT_CNY_1 39
#define LED_OUTPUT_WIFI 12
#define LED_OUTPUT_IOT_HUB 13

/**
 * Instancias 
 * */
WiFiClient espClient; // Instancia que contiene la configuracion WiFi

/**
 * Variables WiFi
 * */
const char *ssid = "ssid";
const char *password_wifi = "password_wifi";
const char *connectionString = "HostName=demo-iot-resource.azure-devices.net;DeviceId=esp32;SharedAccessKey=+RaYowj2AfsRhuifdjwdRXwYcdlwOezuEotnq9SMmK4=";
const char *messageData = "{\"deviceId\":\"%s\", \"topic\":%s, \"data\":%s }";
//****************************************
//******* DECLARACION DE FUNCIONES *******
//****************************************

// Fución que se encarga de realizar una conexion por WIFI
void connectWifi();
// Funcion que se encarga de conectarse con iOT-Hub
void connectToAzureIotHub();
// Funcion que se encarga de enviar un mensaje
void sendSimpleMessageToIotHub(String data);
// Funcion que se encarga de detectar un mensaje de entrada desde iOT-Hub
void messageCallback(const char *payLoad, int size);
// funcion que se encarga de confirmar el envio del mensaje  desde iOT-Hub
static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);

//****************************************
//********** SETUP APPLICATION  **********
//****************************************
void setup()
{
  Serial.begin(9600);
  pinMode(LED_OUTPUT_WIFI, OUTPUT);
  pinMode(LED_OUTPUT_IOT_HUB, OUTPUT);
  connectWifi();
  connectToAzureIotHub();
}

//****************************************
//********** LOOP APPLICATION  ***********
//****************************************
void loop()
{
  if (WiFi.isConnected())
  {
    sendSimpleMessageToIotHub(" Mensaje 1");
  }
  else
  {
    digitalWrite(LED_OUTPUT_WIFI, LOW);
    digitalWrite(LED_OUTPUT_IOT_HUB, LOW);
    connectWifi();
    connectToAzureIotHub();
  }
}

//****************************************
//******* DEFINICIÓN FUNCIONES  **********
//****************************************

/**
 * Funcion que se encarga de conectarse a internet
 * */
void connectWifi()
{
  Serial.println("Connect to WIFI...");
  WiFi.begin(ssid, password_wifi);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Status WIFI: " + WiFi.status());
  }
  Serial.println(WiFi.localIP().toString());
  digitalWrite(LED_OUTPUT_WIFI, HIGH);
  randomSeed(micros());
}

/**
 * Funcion que se encarga de conextarse a Azure IOT-HUB
 * */
void connectToAzureIotHub()
{

  Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "iot");
  if (!Esp32MQTTClient_Init((const uint8_t *)connectionString))
  {
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  Esp32MQTTClient_SetSendConfirmationCallback(sendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(messageCallback);
  Esp32MQTTClient_Check(true);

  digitalWrite(LED_OUTPUT_IOT_HUB, HIGH);
  randomSeed(micros());
  sendSimpleMessageToIotHub("Start");
}

/**
 * Funcion que se encarga de enviar un mensaje a iot-hub
 * */
void sendSimpleMessageToIotHub(String data)
{
  char messagePayload[256];
  snprintf(messagePayload, 256, messageData, "esp32", "sensors", data);
  Serial.print("Sending data: ");
  Serial.println(messagePayload);
  EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
  Esp32MQTTClient_Event_AddProp(message, "test", "true");
  Esp32MQTTClient_SendEventInstance(message);
}

/**
 * Funcion que se encarga de confirmar el envío de un mensaje
 * */
static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

/**
 * Funcion que se activa cuando hay un mensaje de entrada
 * */
void messageCallback(const char *payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}