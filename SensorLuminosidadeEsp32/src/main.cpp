#include <Arduino.h>
#include <WiFi.h> // Biblioteca para conexão Wi-Fi

// Configurações de Wi-Fi
const char* ssid = "SEU_SSID"; // Insira o nome da rede Wi-Fi
const char* password = "SUA_SENHA"; // Insira a senha da rede Wi-Fi

// Definindo os pinos dos LDRs e o limiar de sombra
#define LDR_PIN1 34 // Pino de entrada analógica para o LDR1
#define LDR_PIN2 35 // Pino de entrada analógica para o LDR2
#define LDR_PIN3 32 // Pino de entrada analógica para o LDR3
#define LDR_PIN4 33 // Pino de entrada analógica para o LDR4 (trocado com o NTC)
#define THRESHOLD 1500 // Limiar para diferenciar luz de sombra

// Definindo os pinos dos LEDs externos
#define LED_PIN1 25 // Pino digital onde o LED1 está conectado
#define LED_PIN2 26 // Pino digital onde o LED2 está conectado
#define LED_PIN3 27 // Pino digital onde o LED3 está conectado
#define LED_PIN4 14 // Pino digital onde o LED4 está conectado

// Definição do pino do sensor NTC (termistor)
#define NTC_PIN 39 // Pino analógico onde o NTC está conectado (trocado com o LDR4)

// Variáveis para controle de tempo
unsigned long lastReadTime = 0; // Armazena o último momento de leitura dos sensores
unsigned long readInterval = 100; // Intervalo para leitura dos sensores (em milissegundos)

void conectarWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

float lerNTC(int pin) {
  int rawValue = analogRead(pin); // Lê o valor analógico do pino
  float resistance = (4095.0 / rawValue) - 1.0; // Cálculo da resistência
  resistance = 10000.0 / resistance; // NTC de 10k (NTC10k)
  
  float temperature = resistance / 10000.0; // R/Ro (onde Ro = 10k)
  temperature = log(temperature); // ln(R/Ro)
  temperature = temperature / 3950.0; // 1/B (onde B é a constante de beta)
  temperature += 1.0 / (25.0 + 273.15); // 1/To (25°C em Kelvin)
  temperature = 1.0 / temperature; // Inverso da soma
  temperature -= 273.15; // Converte de Kelvin para Celsius
  return temperature; // Retorna a temperatura em Celsius
}

void setup() {
  // Conecta ao Wi-Fi
  conectarWiFi();

  // Define os pinos dos LEDs como saída
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);

  // Define os pinos dos LDRs como entrada
  pinMode(LDR_PIN1, INPUT);
  pinMode(LDR_PIN2, INPUT);
  pinMode(LDR_PIN3, INPUT);
  pinMode(LDR_PIN4, INPUT);

  // Define o pino do NTC como entrada (opcional, pois analogRead já faz isso automaticamente)
  pinMode(NTC_PIN, INPUT);
}

void loop() {
  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis(); 

    int ldrValue1 = analogRead(LDR_PIN1);
    int ldrValue2 = analogRead(LDR_PIN2);
    int ldrValue3 = analogRead(LDR_PIN3);
    int ldrValue4 = analogRead(LDR_PIN4);

    float temperatura = lerNTC(NTC_PIN);

    if (ldrValue1 < THRESHOLD) {
      digitalWrite(LED_PIN1, HIGH);
    } else {
      digitalWrite(LED_PIN1, LOW);
    }

    if (ldrValue2 < THRESHOLD) {
      digitalWrite(LED_PIN2, HIGH);
    } else {
      digitalWrite(LED_PIN2, LOW);
    }

    if (ldrValue3 < THRESHOLD) {
      digitalWrite(LED_PIN3, HIGH);
    } else {
      digitalWrite(LED_PIN3, LOW);
    }

    if (ldrValue4 < THRESHOLD) {
      digitalWrite(LED_PIN4, HIGH);
    } else {
      digitalWrite(LED_PIN4, LOW);
    }
  }
}
