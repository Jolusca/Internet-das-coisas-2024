#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h" // Biblioteca para conexão Wi-Fi

// Configurações de Wi-Fi
//const char* ssid = "Jlucas"; // Insira o nome da rede Wi-Fi
//const char* password = "jlucas2002"; // Insira a senha da rede Wi-Fi

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// Definindo os pinos dos LDRs e o limiar de sombra
#define LDR_PIN1 34 // Pino de entrada analógica para o LDR1 era 34
#define LDR_PIN2 35 // Pino de entrada analógica para o LDR2
#define LDR_PIN3 32 // Pino de entrada analógica para o LDR3
#define LDR_PIN4 33 // Pino de entrada analógica para o LDR4
#define THRESHOLD 700 // Limiar para diferenciar luz de sombra

// Definindo os pinos dos LEDs externos
#define LED_PIN1 25 // Pino digital onde o LED1 está conectado
#define LED_PIN2 26 // Pino digital onde o LED2 está conectado
#define LED_PIN3 27 // Pino digital onde o LED3 está conectado
#define LED_PIN4 14 // Pino digital onde o LED4 está conectado
//#define LED_TEMP 13 // Novo LED que será ativado caso a temperatura ultrapasse 50 graus

// Definição do pino do sensor NTC (termistor)
#define NTC_PIN 39 // Pino analógico onde o NTC está conectado

// Pino para leitura da tensão
#define VOLTAGE_PIN 36

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

//Definindo resistores para o divisor de tensao
const float R1 = 34000.0;//33k
const float R2 = 9800.0;//10k
float mensagem[6]; // Vetor para os valores LDRs, Temperatura e Tensão

// Variáveis para controle de tempo
unsigned long lastReadTime = 0; // Armazena o último momento de leitura dos sensores
unsigned long readInterval = 100; // Intervalo para leitura dos sensores (em milissegundos)

void conectarWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Conectando ao Wi-Fi...");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) { // Tenta por 10 segundos
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFalha ao conectar ao Wi-Fi!");
        return;
    }
    Serial.println("\nConectado ao Wi-Fi!");
}


void connectAWS() {
  conectarWiFi();

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  //client.setCallback(messageHandler);
  
  Serial.println("Conectando ao AWS IoT...");
while (!client.connect(THINGNAME)) {
  Serial.print("Falha ao conectar. Estado: ");
  Serial.println(client.state());
  delay(1000);
}
  
  if (!client.connected()) {
    Serial.println("Falha ao conectar no AWS IoT!");
    return;
  }
  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("Conectado ao AWS IoT e inscrito no tópico!");
}

void publishMessage(float mensagem[]) {
  StaticJsonDocument<200> doc; // Cria o documento JSON
  
  // Atribui valores aos campos do JSON
  doc["LDR1"] = mensagem[0];
  doc["LDR2"] = mensagem[1];
  doc["LDR3"] = mensagem[2];
  doc["LDR4"] = mensagem[3];
  doc["Temperatura"] = mensagem[4];
  doc["Tensao"] = mensagem[5];

  char jsonBuffer[512]; // Buffer para armazenar o JSON gerado
  serializeJson(doc, jsonBuffer); // Converte o JSON para string e armazena no buffer

  // Publica o JSON no tópico MQTT
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer); 
  Serial.print("Mensagem enviada para o AWS: ");
  Serial.println(jsonBuffer); // Imprime o JSON enviado para o console
}

//metodo para receber mensagens da aws
void messageHandler(char* topic, byte* payload, unsigned int length){
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

float lerNTC(int pin) {
  int rawValue = analogRead(pin); 
  if (rawValue == 0) return -999; // Caso erro na leitura, retorna -999
  float resistance = (4095.0 / rawValue) - 1.0; 
  resistance = 10000.0 / resistance; 
  float temperature = resistance / 10000.0; 
  temperature = log(temperature); 
  temperature = temperature / 3650.0; 
  temperature += 1.0 / (25.0 + 273.15); 
  temperature = 1.0 / temperature; 
  temperature -= 273.15; 
  return temperature; 
}

float lerTensao(int pin) {
  int rawValue = analogRead(pin); // Lê o valor analógico do pino
  float voltage = rawValue * (3.3 / 4095.0); // Converte o valor lido para tensão
  float realVoltage = voltage * ((R1 + R2) / R2); // Converte a tensão medida para a tensão real da fonte
  return realVoltage;
}


void imprimirMensagem(float mensagem[]){
  Serial.print("LDRs: ");
  Serial.print(mensagem[0]);
  Serial.print(" ");
  Serial.print(mensagem[1]);
  Serial.print(" ");
  Serial.print(mensagem[2]);
  Serial.print(" ");
  Serial.print(mensagem[3]);
  Serial.print(" | Temperatura: ");
  Serial.print(mensagem[4], 2);
  Serial.print("C | Tensao: ");
  Serial.print(mensagem[5], 2);
  Serial.println("V");

  //atraso de 2 seg
  //delay(2000);
}

void setup() {
  // Conecta ao Wi-Fi
  //conectarWiFi();

  // Define os pinos dos LEDs como saída
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);
  //pinMode(LED_TEMP, OUTPUT); // Configura o pino do LED de temperatura como saída

  // Define os pinos dos LDRs como entrada
  pinMode(LDR_PIN1, INPUT);
  pinMode(LDR_PIN2, INPUT);
  pinMode(LDR_PIN3, INPUT);
  pinMode(LDR_PIN4, INPUT);

  // Define o pino do NTC como entrada (opcional, pois analogRead já faz isso automaticamente)
  pinMode(NTC_PIN, INPUT);
  
  // Define o pino de tensão como entrada
  pinMode(VOLTAGE_PIN, INPUT);

  // Inicializa a comunicação serial para depuração
  Serial.begin(115200);
  connectAWS();
}

void loop() {
  if (millis() - lastReadTime >= readInterval) {

    lastReadTime = millis();  

    int ldrValue1 = analogRead(LDR_PIN1);
    int ldrValue2 = analogRead(LDR_PIN2);
    int ldrValue3 = analogRead(LDR_PIN3);
    int ldrValue4 = analogRead(LDR_PIN4);

    float temperatura = lerNTC(NTC_PIN);
    float tensao = lerTensao(VOLTAGE_PIN);

    float mensagem[6];

    mensagem[0] = (ldrValue1 < THRESHOLD) ? 0 : 1;
    mensagem[1] = (ldrValue2 < THRESHOLD) ? 0 : 1;
    mensagem[2] = (ldrValue3 < THRESHOLD) ? 0 : 1;
    mensagem[3] = (ldrValue4 < THRESHOLD) ? 0 : 1;
    mensagem[4] = temperatura;
    mensagem[5] = tensao;

    if (mensagem[0] == 0) {
      digitalWrite(LED_PIN1, HIGH);
    } else {
      digitalWrite(LED_PIN1, LOW);
    }

    if (mensagem[1] == 0) {
      digitalWrite(LED_PIN2, HIGH);
    } else {
      digitalWrite(LED_PIN2, LOW);
    }

    if (mensagem[2] == 0) {
      digitalWrite(LED_PIN3, HIGH);
    } else {
      digitalWrite(LED_PIN3, LOW);
    }

    if (mensagem[3] == 0) {
      digitalWrite(LED_PIN4, HIGH);
    } else {
      digitalWrite(LED_PIN4, LOW);
    }

  // Verifica se a temperatura está acima de 50 graus Celsius
  /*if (temperatura > 50.0) { 
    digitalWrite(LED_TEMP, HIGH); // Liga o LED de temperatura
  } else {
    digitalWrite(LED_TEMP, LOW); // Desliga o LED de temperatura
  }*/

  // Imprime o vetor no console serial para depuração
  imprimirMensagem(mensagem);

  //manda a mensgem para azure
  publishMessage(mensagem);
  client.loop();
  delay(4000);
  }
}