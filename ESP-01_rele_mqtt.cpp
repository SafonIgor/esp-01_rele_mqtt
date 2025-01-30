#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Настройки Wi-Fi
const char* ssid = "ВАШ_SSID";       // Замени на имя своей Wi-Fi сети
const char* password = "ВАШ_ПАРОЛЬ"; // Замени на пароль от Wi-Fi

// Настройки MQTT
const char* mqtt_server = "mqtt.broker.com"; // Адрес MQTT-брокера
const int mqtt_port = 1883;                  // Порт MQTT-брокера
const char* mqtt_user = "ВАШ_ЛОГИН";        // Логин MQTT (если требуется)
const char* mqtt_password = "ВАШ_ПАРОЛЬ";   // Пароль MQTT (если требуется)

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN 2 // GPIO2 на ESP-01

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Изначально реле выключено

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nПодключено к Wi-Fi!");

  // Настройка MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Подключение к MQTT-брокеру
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Функция обработки входящих сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Сообщение получено [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Управление реле
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY_PIN, HIGH); // Включаем реле
    Serial.println("Реле включено");
  } else if ((char)payload[0] == '0') {
    digitalWrite(RELAY_PIN, LOW); // Выключаем реле
    Serial.println("Реле выключено");
  }
}

// Функция переподключения к MQTT-брокеру
void reconnect() {
  while (!client.connected()) {
    Serial.print("Подключение к MQTT...");
    if (client.connect("ESP01Client", mqtt_user, mqtt_password)) {
      Serial.println("Подключено");
      client.subscribe("relay/control"); // Подписываемся на топик
    } else {
      Serial.print("Ошибка, rc=");
      Serial.print(client.state());
      Serial.println(" Повторная попытка через 5 секунд...");
      delay(5000);
    }
  }
}
