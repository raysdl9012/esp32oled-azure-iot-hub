/**
 * Incluir librerias
 * */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>


/**
 * Definir variables
 * */
#define LED_OUTPUT_WIFI 12
#define LED_OUTPUT_MQTT 13

/**
 * Instancias 
 * */
WiFiClient espClient; // Instancia que contiene la configuracion WiFi
PubSubClient mqttClient(espClient); // Instancia conexion MQTT

/**
 * Variables globales
 * */
boolean sendMessage = false; // Variable que indica si se envio algun mensaje

/**
 * Variables WiFi
 * */
const char *ssid = "ssid"; // Nombre de la red
const char *password_wifi = "password_wifi"; // Contraseña de la red

/**
 * Variables MQTT
 * */
const int mqtt_port = 1883; // Puerto
const char *topic = "test1"; // Topico
const char *mqtt_server = "192.168.20.27"; // IP del servidor BROKER MQTT
const char *username_mqtt = "username_mqtt"; // Usuario del BROKER
const char *password_mqtt = "password_mqtt"; // Contraseña de BROKER


//****************************************
//******* DECLARACION DE FUNCIONES *******
//****************************************

// WIFI
void connectWifi();
// MQTT
void defaultConfigurationMQTT();
void reconnectMQTT();
void callback(char *topic, byte *payload, unsigned int length);

//****************************************
//********** SETUP APPLICATION  **********
//****************************************
void setup()
{
  Serial.begin(115200);
  pinMode(LED_OUTPUT_WIFI, OUTPUT);
  pinMode(LED_OUTPUT_MQTT, OUTPUT);
  defaultConfigurationMQTT();
  connectWifi();
  
}



//****************************************
//********** LOOP APPLICATION  ***********
//****************************************
void loop()
{
  if (WiFi.isConnected())
  {
    if (!mqttClient.connected())
    {
      digitalWrite(LED_OUTPUT_MQTT, LOW);
      reconnectMQTT();
    }
    mqttClient.loop();
    digitalWrite(LED_OUTPUT_MQTT, HIGH);
    if(!sendMessage){
      String messageToSend = "Hola desde ESP32";
      char msg[50];
      Serial.println(msg);
      messageToSend.toCharArray(msg, 50);
      mqttClient.publish(topic, msg);
      sendMessage=true;
    }
  }
  else
  {
    digitalWrite(LED_OUTPUT_WIFI, LOW);
    connectWifi();
  }
}

//****************************************
//******* DEFINICIÓN FUNCIONES  **********
//****************************************

/**
 * Funcion que se encarga de realizar la conexion WiFi
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
  Serial.println("" + WiFi.localIP().toString());
  digitalWrite(LED_OUTPUT_WIFI, HIGH);
  randomSeed(micros());
}

/**
 * Funcion callback que se ejecuta cuando llega un mensaje por MQTT
 * */
void callback(char *topic, byte *payload, unsigned int length)
{
  String dataMessage = "";
  Serial.println(topic);
  for (int i = 0; i < length; i++)
  {
    dataMessage += (char)payload[i];
  }
  dataMessage.trim();
  Serial.println(dataMessage);
}

/**
 * Funcion que se encarga de configurar la conexión MQTT
 * */
void defaultConfigurationMQTT()
{
  Serial.println("Config MQTT: " );
  Serial.print(mqtt_server);
  Serial.print(mqtt_port);
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

/**
 * Funcion que se encarga de realizar la conexión MQTT con el BROKER
 * */
void reconnectMQTT()
{
  while (!mqttClient.connected())
  {
    Serial.println("** Try Connect to MQTT");
    String clientId = "ESP32-"+String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(),username_mqtt,password_mqtt))
    {
      Serial.println("Connected to MQTT");
      mqttClient.subscribe(topic);
      Serial.println("Suscribe to topic to MQTT");  
      Serial.print(topic);  
    }
    else
    {
      Serial.println("MQTT estatus" + mqttClient.state());
      delay(5000);
    }
  }
}