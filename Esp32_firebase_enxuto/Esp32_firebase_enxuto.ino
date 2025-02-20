#include <WiFi.h>         
#include <IOXhop_FirebaseESP32.h>                           
#include <ArduinoJson.h>                   
#include <ESP32Servo.h>
#include <time.h> 
#include <Ticker.h> 
#include <Arduino.h>

#define WIFI_SSID "Fazendo o L"                   
#define WIFI_PASSWORD "aaa12345"         
#define FIREBASE_HOST "https://gesa-project-default-rtdb.firebaseio.com/"    
#define FIREBASE_AUTH "ETBsNtyQDec90XA0axzgoZn4OonLDc9wFmY1wbWc"   

#define LDR_PIN1 34
#define LDR_PIN2 35
#define LDR_PIN3 32
#define LDR_PIN4 33
#define NTC_PIN 39
#define VOLTAGE_PIN 36
#define SERVO_PIN 14
#define LED_PIN 2

const float R1 = 34000.0; // 33k
const float R2 = 9800.0;  // 10k

#define THRESHOLD 600
#define uS_TO_S_FACTOR 1000000  
#define TIME_TO_SLEEP  10      

float mensagem[6]; 
int servoPosition = 90;
Servo myServo;
Ticker ticker;

void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando ao WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        delay(500);
        attempts++;
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi Conectado!" : "\nFalha ao conectar!");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado! Tentando reconectar...");
        connectToWiFi();
    }
    delay(1000);
}

void setup() {
    pinMode(LDR_PIN1, INPUT);
    pinMode(LDR_PIN2, INPUT);
    pinMode(LDR_PIN3, INPUT);
    pinMode(LDR_PIN4, INPUT);
    pinMode(NTC_PIN, INPUT);
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);
    delay(1000);
    connectToWiFi();
    myServo.attach(SERVO_PIN);
    myServo.write(servoPosition);

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Serial.println("Conectado ao Firebase!");

    ticker.attach(10, readSensors);
    // Configura o fuso horário e os servidores NTP (exemplo para GMT-3)
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // Aguarda alguns segundos para sincronizar a hora
    struct tm timeinfo;
    int tentativas = 0;
    while (!getLocalTime(&timeinfo) && tentativas < 10) {
      Serial.println("Obtendo hora...");
      delay(1000);
      tentativas++;
    }
    if(tentativas >= 10) {
      Serial.println("Falha ao sincronizar a hora");
    }else {
      Serial.println("Hora sincronizada com sucesso");
    }
}

void readSensors() {
    int ldrValue1 = analogRead(LDR_PIN1);
    int ldrValue2 = analogRead(LDR_PIN2);
    int ldrValue3 = analogRead(LDR_PIN3);
    int ldrValue4 = analogRead(LDR_PIN4);
    float temperatura = lerNTC(NTC_PIN);
    float tensao = lerTensao(VOLTAGE_PIN);

    mensagem[0] = (ldrValue1 < 600) ? 0 : 1;
    mensagem[1] = (ldrValue2 < 600) ? 0 : 1;
    mensagem[2] = (ldrValue3 < 600) ? 0 : 1;
    mensagem[3] = (ldrValue4 < 600) ? 0 : 1;
    mensagem[4] = temperatura;
    mensagem[5] = tensao;

    Serial.printf("LDRs: %d %d %d %d | Temp: %.2f°C | Tensão: %.2fV\n",
                  (int)mensagem[0], (int)mensagem[1], (int)mensagem[2], (int)mensagem[3], mensagem[4], mensagem[5]);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Falha ao obter a hora atual");
        return;
    }

    char dia[11], horario[9];
    strftime(dia, sizeof(dia), "%Y-%m-%d", &timeinfo);
    strftime(horario, sizeof(horario), "%H:%M:%S", &timeinfo);

    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/LDR1", mensagem[0]);
    Serial.println("Envio LDR1 realizado");
    Serial.println(mensagem[0]);
    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/LDR2", mensagem[1]);
    Serial.println("Envio LDR2 realizado");
    Serial.println(mensagem[1]);
    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/LDR3", mensagem[2]);
    Serial.println("Envio LDR3 realizado");
    Serial.println(mensagem[2]);
    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/LDR4", mensagem[3]);
    Serial.println("Envio LDR4 realizado");
    Serial.println(mensagem[3]);
    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/temp", mensagem[4]);
    Serial.println("Envio temp realizado");
    Serial.println(mensagem[4]);
    Firebase.setFloat("/Leituras/" + String(dia) + "/" + String(horario) + "/tensao", mensagem[5]);
    Serial.println("Envio /tensão realizado");
    Serial.println(mensagem[5]);

    delay(2000);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);

    /*
    Serial.println("Entrando em modo de sono profundo...");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
    */
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
