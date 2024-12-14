#include <Arduino.h>

// Definindo o pino do LDR e o limiar de sombra
#define LDR_PIN1 34           // Pino de entrada analógica para o LDR
#define LDR_PIN2 35
#define LDR_PIN3 32
#define THRESHOLD 1500       // Limiar para diferenciar luz de sombra (ajuste conforme o ambiente)

// Definindo o pino do LED externo
#define LED_PIN1 25           // Pino digital onde o LED externo está conectado
#define LED_PIN2 26          // Pino digital onde o LED externo está conectado
#define LED_PIN3 27           // Pino digital onde o LED externo está conectado


void setup() {
  // Inicializa a porta serial para monitoramento
  Serial.begin(115200);

  // Define o pino do LED como saída
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);

  // Define o pino do LDR como entrada (opcional, pois analogRead já faz isso automaticamente)
  pinMode(LDR_PIN1, INPUT);
  pinMode(LDR_PIN2, INPUT);
  pinMode(LDR_PIN3, INPUT);
}

void loop() {
  // Lê o valor analógico do LDR (0 a 4095 no ESP32)
  int ldrValue1 = analogRead(LDR_PIN1);
  int ldrValue2 = analogRead(LDR_PIN2);
  int ldrValue3 = analogRead(LDR_PIN3);

  
  // Exibe o valor lido no monitor serial
  Serial.print("Valor do LDR: ");
  Serial.println(ldrValue1);
  
  // Verifica se o valor está abaixo do limiar (sombra) e aciona o LED
  if (ldrValue1 < THRESHOLD) {
    digitalWrite(LED_PIN1, HIGH); // Liga o LED
  } else {
    digitalWrite(LED_PIN1, LOW);
  }
    

  if (ldrValue2 < THRESHOLD) {
    digitalWrite(LED_PIN2, HIGH); // Liga o LED
  } else {
    digitalWrite(LED_PIN2, LOW);
  }
    // Atraso para estabilizar a leitura

  if (ldrValue3 < THRESHOLD) {
    digitalWrite(LED_PIN3, HIGH); // Liga o LED
  } else {
    digitalWrite(LED_PIN3, LOW);
  }
    // Atraso para estabilizar a leitura
    delay(100);
}