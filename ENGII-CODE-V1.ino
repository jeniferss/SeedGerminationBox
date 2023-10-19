// Biblioteca utilizada para fazer a comunicacao com o modulo I2C e o display LCD 20x4
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// BibliotecaS utilizadaS para fazer a comunicacao com o sensor de umidade e temperatura
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Bibliotecas utilizadas para fazer a comunicacao com o servo motor
#include <Servo.h>

int ADDRESS = 0x27;  // Endereço do display

int COLUMNS = 20;  // Numero de colunas do display
int LINES = 4;     // Numero de linhas do display

int DHT11_PIN = 5;  // Pino de conexao com o sensor de humidade e temperatura

int MOTOR_PIN = 8;   // Pino de conexao com o servo motor
int BUZZER_PIN = 9;  // Pino de conexao com o buzzer

int MAX_TEMPERATURE = 23;  // Temperatura maxima
int MIN_HUMIDITY = 20;     // Humidade minima

LiquidCrystal_I2C lcd(ADDRESS, COLUMNS, LINES);  // Inicializa o display para ser usado com o modulo I2C
DHT_Unified dht(DHT11_PIN, DHT11);               // Inicializa o sensor de humidade e temperatura
Servo servoMotor;                                // Inicializa o servo motor

int DHT11_DELAY;  // Define o delay de leitura para o sensor de umidade e temperatura

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Definir BUZZER_PIN como tipo de saida

  servoMotor.attach(MOTOR_PIN);  // Definir que o motor esta ligado em MOTOR_PIN = 8

  lcd.init();       // Inicia a comunicação com o display
  lcd.backlight();  // Liga a luz de fundo do display
  lcd.clear();      // Limpa o texto na tela do display

  dht.begin();  // Inicia a comunicação com o sendor de temperatura e umidade
  sensor_t sensor;
  int DHT11_DELAY = sensor.min_delay / 1000;  // Atribui o valor do delay de leitura para o sensor de umidade e temperatura
}

void loop() {
  sensors_event_t event;

  int temperature = readDHT11Temperature(event);
  int humidity = readDHT11Humidity(event);

  delay(DHT11_DELAY);
  displayTemperatureAndHumidity(temperature, humidity);
}

int readDHT11Temperature(sensors_event_t event) {
  if (!isnan(event.temperature))
    return dht.temperature().getEvent(&event);
  return 0;
}

int readDHT11Humidity(sensors_event_t event) {
  if (!isnan(event.relative_humidity))
    return dht.humidity().getEvent(&event);
  return 0;
}

void displayTemperatureAndHumidity(int temperature, int humidity) {
  if (temperature == 0 || humidity == 0) {
    lcd.setCursor(0, 0);
    lcd.print("ERRO DE MEDICAO");
  }

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

  for (int i = 0; i <= 4; i++) {
    if (hour == hours[i]) {
      for (int j = 0; j <= 25; j++) {
        servoMotor.write(180);  // Move o servo para o angulo de 180 graus
        delay(500);
        servoMotor.write(0);  // Move o servo para o angulo de 0 graus
        delay(500);
      }
      break;
    }
  }
}
