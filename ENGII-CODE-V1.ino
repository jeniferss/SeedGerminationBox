// Biblioteca utilizada para fazer a comunicacao com o modulo I2C e o display LCD 20x4
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Biblioteca utilizada para fazer a comunicacao com o relogio
//#include <DS3231.h>

// Biblioteca utilizada para fazer a comunicacao com o servo motor
#include <Servo.h>

// Biblioteca utilizada para fazer a comunicacao com o sensor de umidade e temperatura
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define DHT11PIN 5

int MOTOR_PIN = 8;   // Pino de conexao com o servo motor
int BUZZER_PIN = 9;  // Pino de conexao com o buzzer

int MAX_TEMPERATURE = 20;  // Temperatura maxima
int MIN_HUMIDITY = 20;     // Umidade minima

DHT_Unified dht(DHT11PIN, DHTTYPE);
uint32_t delayDHT11;

// Inicializa o display para ser usado com o modulo I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Inicializa o relogio
//DS3231 RTCClock;

// Inicializa o servo motor
Servo servoMotor;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  servoMotor.attach(MOTOR_PIN);

  lcd.init();       // Inicia a comunicação com o display
  lcd.backlight();  // Liga a luz de fundo do display
  lcd.clear();      // Limpa o texto na tela do display

  Serial.begin(9600);

  dht.begin();

  sensor_t sensor;
  delayDHT11 = sensor.min_delay / 1000;

  // RTCClock.setTime(0, 0, 0);     // Definir horario inicial do relogio como 00:00
  // RTCClock.setDate(1, 1, 2023);  // Definir data inicial do relogio como 01/01/2023
}

void loop() {
    delay(delayDHT11);

  sensors_event_t event;

  dht.temperature().getEvent(&event);
  int temperature = event.temperature;

  dht.humidity().getEvent(&event);
  int humidity = event.relative_humidity;

  if (!isnan(temperature) && !isnan(humidity)) {
    displayTemperatureAndHumidity(temperature, humidity);
    triggerBuzzer(temperature, humidity);
  }

}

void displayTemperatureAndHumidity(int temperature, int humidity) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURA: ");

  lcd.setCursor(13, 0);
  lcd.print(temperature);

  lcd.setCursor(16, 0);
  lcd.print("*C");

  lcd.setCursor(0, 2);
  lcd.print("HUMIDADE: ");

  lcd.setCursor(10, 2);
  lcd.print(humidity);

  lcd.setCursor(13, 2);
  lcd.print("%");
}

void triggerBuzzer(int temperature, int humidity) {
  if (temperature > MAX_TEMPERATURE || humidity < MIN_HUMIDITY) {
    for (int i = 0; i <= 3; i++) {
      tone(BUZZER_PIN, 30, 1000);
      delay(1000);
      noTone(BUZZER_PIN);
    }
  }
}

void triggerMotor(int hour) {
  const byte hours[] = { 4, 10, 16, 22 };

  for (int i = 0; i <= 4; i++) {
    if (hour == hours[i]) {
      for (int j = 0; j <= 5; j++) {
        servoMotor.write(180);  // Move o servo para o angulo de 90 graus
        delay(500);
        servoMotor.write(0);  // Move o servo para o angulo de 0 graus
        delay(500);
      }
      break;
    }
  }
}
