#include <WiFi.h>         
#include <IOXhop_FirebaseESP32.h>                           
#include <ArduinoJson.h>                   
#include <ESP32Servo.h>

#define WIFI_SSID "Fazendo o L"                   
#define WIFI_PASSWORD "aaa12345"         
#define FIREBASE_HOST "https://gesa-project-default-rtdb.firebaseio.com/"    
#define FIREBASE_AUTH "ETBsNtyQDec90XA0axzgoZn4OonLDc9wFmY1wbWc"   

// Definições de Tópicos e Sensores
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define LDR_PIN1 34
#define LDR_PIN2 35
#define LDR_PIN3 32
#define LDR_PIN4 33
#define THRESHOLD 600
#define NTC_PIN 39
#define VOLTAGE_PIN 36
#define SERVO_PIN 14

Servo myServo;

// Novo LED que será ativado caso a temperatura ultrapasse 50 graus
/*
#define LED_TEMP 13 
*/

// ################### Definindo os pinos dos LEDs externos
/*
#define LED_PIN1 25 // Pino digital onde o LED1 está conectado
#define LED_PIN2 26 // Pino digital onde o LED2 está conectado
#define LED_PIN3 27 // Pino digital onde o LED3 está conectado
#define LED_PIN4 14 // Pino digital onde o LED4 está conectado
*/

// Resistores para divisor de tensão
const float R1 = 34000.0; //33k
const float R2 = 9800.0;  //10k

// Variáveis globais
float mensagem[6]; // Vetor para os valores LDRs, Temperatura e Tensão
unsigned long lastReadTime = 0; // Armazena o último momento de leitura dos sensores
unsigned long readInterval = 100; // Intervalo para leitura dos sensores (em milissegundos)
int servoPosition = 90; // Posição inicial do servo (centro)

void conectarWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        // Já está conectado, não tenta conectar novamente
        return;
    }

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

// Definições para sincronização de horário
#define GMT_OFFSET -3 * 3600 // Fuso horário de Brasília (GMT-3)
#define DAYLIGHT_OFFSET 0    // Sem horário de verão
#define NTP_SERVER "pool.ntp.org" // Servidor NTP

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
  Serial.println("V");}
void setup() {
  // Inicializa a comunicação serial para depuração
  Serial.begin(115200);

  // Conecta ao Wi-Fi
  conectarWiFi();

  // Inicializa o Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
  Serial.println("Sincronizando horário...");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
      Serial.println("Falha ao obter tempo!");
      delay(1000);
  }
  Serial.println("Horário sincronizado!");

  // Configura os pinos dos LEDs como saída (se ainda forem usados)
  /*
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);1
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);
  */

  // Configura os pinos dos sensores como entrada
  pinMode(LDR_PIN1, INPUT);
  pinMode(LDR_PIN2, INPUT);
  pinMode(LDR_PIN3, INPUT);
  pinMode(LDR_PIN4, INPUT);
  pinMode(NTC_PIN, INPUT);
  pinMode(VOLTAGE_PIN, INPUT);

  // Inicializa o servo
  myServo.attach(SERVO_PIN);
}

void loop() {
    lerSensores(); // Lê os sensores

    // Obter o horário atual
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Erro ao obter o tempo!");
        return;
    }
    char timestampStr[30];
    strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();

    data["ldr1"] = mensagem[0];
    data["ldr2"] = mensagem[1];
    data["ldr3"] = mensagem[2];
    data["ldr4"] = mensagem[3];
    data["temperature"] = mensagem[4];
    data["voltagem"] = mensagem[5];
    data["timestamp"] = timestampStr;  // Mantendo o timestamp no mesmo formato
    // Converter para string JSON
    String jsonString;
    data.printTo(jsonString);
    // Enviar para o Firebase com push (gera um ID único)
    if (Firebase.pushString("sensores", jsonString)) {
        Serial.println("Dados enviados ao Firebase com sucesso!");
    } else {
        Serial.print("Erro ao enviar dados: ");
        Serial.println(Firebase.error());
    }

    imprimirMensagem(mensagem);
    delay(5000); // Aguarda 2 segundos antes da próxima leitura
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

void lerSensores() {
    mensagem[0] = (analogRead(LDR_PIN1) < THRESHOLD) ? 0 : 1;
    mensagem[1] = (analogRead(LDR_PIN2) < THRESHOLD) ? 0 : 1;
    mensagem[2] = (analogRead(LDR_PIN3) < THRESHOLD) ? 0 : 1;
    mensagem[3] = (analogRead(LDR_PIN4) < THRESHOLD) ? 0 : 1;
    mensagem[4] = lerNTC(NTC_PIN);
    mensagem[5] = lerTensao(VOLTAGE_PIN);
}
