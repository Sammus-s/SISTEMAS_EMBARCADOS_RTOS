#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "";
const char *password = "";
const char *mqtt_server = "127.0.0.1";

//cria objetos para conexão e uso do MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const int Relay = D1;

//conecta a rede setada nas constantes ssid password mqtt_server
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//age como o broker, recebe um tópico e uma mensagem de um publisher, monta a mensagem e encaminha ela para o assinante
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++)
  {
    message += ((char)payload[i]);
  }
  Serial.println(message);

  if (String(topic) == "ControleRelay")
  {
    if (message == "ON")
    {
      digitalWrite(Relay, LOW);
    }
    else if (message == "OFF")
    {
      digitalWrite(Relay, HIGH);
    }
  }
}

//função de start do NodeMCU, conexão e inscrição no tópico do protocolo MQTT
void setup()
{
  pinMode(Relay, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//em caso de queda ele reconcta a rede
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      client.subscribe("ControleRelay");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//função de repetição do NodeMCU
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}