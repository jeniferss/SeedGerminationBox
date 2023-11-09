// Biblioteca utilizada para fazer a comunicacao com o modulo I2C e o display LCD 20x4
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Biblioteca utilizada para fazer a comunicacao com o relogio
#include <DS3231.h>

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

int TARGET_MINUTE = 5;
byte hours[6] = { 'x', 'x', 'x', 'x', 'x', 'x' };

int MAX_TEMPERATURE = 32;  // Temperatura maxima
int MIN_HUMIDITY = 40;     // Umidade minima

DHT_Unified dht(DHT11PIN, DHTTYPE);
uint32_t delayDHT11;

// Inicializa o display para ser usado com o modulo I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Inicializa o relogio
RTClib rtc;

// Inicializa o servo motor
Servo servoMotor;
bool hasServoNotRunned;

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

  primeSpray();

  hasServoNotRunned = true;
}

void loop() {
  delay(3000);

  sensors_event_t event;

  dht.temperature().getEvent(&event);
  int temperature = event.temperature;

  dht.humidity().getEvent(&event);
  int humidity = event.relative_humidity;

  if (!isnan(temperature) && !isnan(humidity)) {
    displayTemperatureAndHumidity(temperature, humidity);
    triggerBuzzer(temperature, humidity);
    defineHoursToRun(temperature, humidity);
  }

  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.println("");

  // Resetar horas de ativacao do motor
  if (hour == 0) {
    resetHours();
  }

  triggerMotor(hour, minute);
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
  lcd.print("UMIDADE: ");

  lcd.setCursor(9, 2);
  lcd.print(humidity);

  lcd.setCursor(12, 2);
  lcd.print("%");
}

void triggerBuzzer(int temperature, int humidity) {
  if (temperature > MAX_TEMPERATURE || humidity < MIN_HUMIDITY) {
    for (int i = 0; i <= 3; i++) {
      tone(BUZZER_PIN, 30, 1000);
      noTone(BUZZER_PIN);
      delay(1000);
    }
  }
}

void resetHours() {
  for (int i = 0; i < 6; i++) {
    hours[i] = 'x';
  }
}

void triggerMotor(int hour, int minute) {
  bool turnMotorOn = false;

  for (int i = 0; i < 6; i++) {
    if (hour == hours[i] && minute == TARGET_MINUTE) {
      turnMotorOn = true;
      break;
    }
  }

  if (!turnMotorOn && !hasServoNotRunned) {
    hasServoNotRunned = true;
  }

  if (turnMotorOn && hasServoNotRunned) {
    moveServo(5);
    hasServoNotRunned = false;
  }
}

void primeSpray() {
  moveServo(6);
}

void moveServo(int sprayQuantity) {
  for (int j = 0; j < sprayQuantity; j++) {
    servoMotor.write(0);
    delay(500);
    servoMotor.write(180);
    delay(500);
  }
}

void defineHoursToRun(int humidity, int temperature) {
  if (hours[0] == 'x') {
    if (temperature <= 18 || (humidity > 60)) {
      hours[0] = 0;
      hours[1] = 8;
      hours[2] = 16;
    }
    if (18 < temperature < 30 || (40 <= humidity <= 60)) {
      hours[0] = 0;
      hours[1] = 6;
      hours[2] = 12;
      hours[3] = 18;
    }
    if (temperature >= 30 || humidity < 40) {
      hours[0] = 0;
      hours[1] = 4;
      hours[2] = 8;
      hours[3] = 12;
      hours[4] = 16;
      hours[5] = 20;
    }
  }
}
