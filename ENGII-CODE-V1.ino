
#include <Wire.h>               // Biblioteca utilizada para fazer a comunicacao com o modulo I2C
#include <LiquidCrystal_I2C.h>  // Biblioteca utilizada para fazer a comunicação com o display LCD 20x4
#include <dht.h>                // Biblioteca utilizada para fazer a comunicacao com o sensor de humidade e temperatura
#include <DS3231.h>             // Biblioteca utilizada para fazer a comunicacao com o relogio
#include <Servo.h>              // Biblioteca utilizada para fazer a comunicacao com o servo motor

#define ADDRESS 0x27;  // Endereço do display

#define COLUMNS = 20;  // Numero de colunas do display
#define LINES = 4;     // Numero de linhas do display

#define DHT11_PIN 5;  // Pino de conexao com o sensor de humidade e temperatura

int MOTOR_PIN = 8;   // Pino de conexao com o servo motor
int BUZZER_PIN = 9;  // Pino de conexao com o buzzer

int MAX_TEMPERATURE = 23;  // Temperatura maxima
int MIN_HUMIDITY = 20;     // Humidade minima

LiquidCrystal_I2C lcd(ADDRESS, COLUMNS, LINES);  // Inicializa o display para ser usado com o modulo I2C
dht DHT;                                         // Inicializa o sensor de humidade e temperatura
DS3231 RTCClock;                                 // Inicializa o relogio
Servo servoMotor;                                // Inicializa o servo motor

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Definir BUZZER_PIN como tipo de saida

  servoMotor.attach(MOTOR_PIN);  // Definir que o motor esta ligado em MOTOR_PIN = 8

  lcd.init();       // Inicia a comunicação com o display
  lcd.backlight();  // Liga a luz de fundo do display
  lcd.clear();      // Limpa o texto na tela do display

  RTCClock.setTime(0, 0, 0);     // Definir horario inicial do relogio como 00:00
  RTCClock.setDate(1, 1, 2023);  // Definir data inicial do relogio como 01/01/2023
}

void loop() {
  DHT.read11(DHT11_PIN);
  int temperature = DHT.temperature;  // Le o valor de temperatura
  int humidity = DHT.humidity;        // Le o valor de humidade

  int alarmHour = RTCClock.getHour();  // Le o valor da hora no alarme

  displayTemperatureAndHumidity(temperature, humidity);  // Mostra os valores de temperatura e humidade no display
  triggerBuzzer(temperature, humidity);                  // Ativa o buzzer baseado nos valores de temperatura e humidade
  triggerMotor(alarmHour);                               // Ativa o motor baseado no valor da hora
}

void displayTemperatureAndHumidity(int temperature, int humidity) {
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURA: ");

  lcd.setCursor(13, 0);
  lcd.print(temperature);

  lcd.setCursor(16, 0);
  lcd.print("°C");

  lcd.setCursor(0, 2);
  lcd.print("HUMIDADE: ");

  lcd.setCursor(11, 2);
  lcd.print(humidity);

  delay(1000);
  lcd.clear();
}

void triggerBuzzer(int temperature, int humidity) {
  if (temperature > MAX_TEMPERATURE || humidity < MIN_HUMIDITY) {
    for (int i; i <= 3; i++) {
      tone(BUZZER_PIN, 500, 1000);
      delay(1000);
      noTone(BUZZER_PIN);
    }
  }
}

void triggerMotor(int hour) {
  const byte hours[] = { 4, 10, 16, 22 };

  for (i = 0; i <= 4; i++) {
    if (hour == hours[i]) {
      for (j = 0; j <= 5; j++) {
        servoMotor.write(180);  // Move o servo para o angulo de 90 graus
        delay(500);
        servoMotor.write(0);  // Move o servo para o angulo de 0 graus
        delay(500);
      }
      break;
    }
  }
}
