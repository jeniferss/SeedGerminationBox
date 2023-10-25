#include <Wire.h>               // Biblioteca utilizada para fazer a comunicacao com o modulo I2C
#include <LiquidCrystal_I2C.h>  // Biblioteca utilizada para fazer a comunicação com o display LCD 20x4

#include <DS3231.h>  // Biblioteca utilizada para fazer a comunicacao com o relogio
#include <Servo.h>   // Biblioteca utilizada para fazer a comunicacao com o servo motor

// Biblioteca utilizada para fazer a comunicacao com o sensor de umidade e temperatura
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#define DHTTYPE DHT11
#define DHT11PIN 5

DHT_Unified dht(DHT11PIN, DHTTYPE);
uint32_t delayDHT11;

int ADRESS = 0x27;  // Endereço do display

int COLUMNS = 20;  // Numero de colunas do display
int LINES = 4;     // Numero de linhas do display

int MOTOR_PIN = 8;   // Pino de conexao com o servo motor
int BUZZER_PIN = 9;  // Pino de conexao com o buzzer

int MAX_TEMPERATURE = 23;  // Temperatura maxima
int MIN_HUMIDITY = 20;     // Humidade minima

LiquidCrystal_I2C lcd(ADRESS, COLUMNS, LINES);  // Inicializa o display para ser usado com o modulo I2C

DS3231 RTCClock;                                // Inicializa o relogio
Servo servoMotor;                               // Inicializa o servo motor

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Definir BUZZER_PIN como tipo de saida

  servoMotor.attach(MOTOR_PIN);  // Definir que o motor esta ligado em MOTOR_PIN = 8

  // lcd.init();       // Inicia a comunicação com o display
  // lcd.backlight();  // Liga a luz de fundo do display
  // lcd.clear();      // Limpa o texto na tela do display

  // Serial.begin(9600);

  // dht.begin();

  // sensor_t sensor;
  // delayDHT11 = sensor.min_delay / 1000;

  // RTCClock.setTime(0, 0, 0);     // Definir horario inicial do relogio como 00:00
  // RTCClock.setDate(1, 1, 2023);  // Definir data inicial do relogio como 01/01/2023
}

void loop() {

  // readTemperatureAndHumidity();

  servoMotor.write(270);  // Move o servo para o angulo de 90 graus
  delay(1000);
  servoMotor.write(0);  // Move o servo para o angulo de 0 graus
  delay(1000);


  // // int alarmHour = RTCClock.getHour();  // Le o valor da hora no alarme

  // displayTemperatureAndHumidity(temperature, humidity);  // Mostra os valores de temperatura e humidade no display
  // triggerBuzzer(temperature, humidity);                  // Ativa o buzzer baseado nos valores de temperatura e humidade
  // triggerMotor(4);                               // Ativa o motor baseado no valor da hora
  // servoMotor.write(0);  // Move o servo para o angulo de 90 graus
  //       delay(1000);
  //       servoMotor.write(180);  // Move o servo para o angulo de 0 graus
  //       delay(1000);
}

int readTemperatureAndHumidity() {
  delay(delayDHT11);

  sensors_event_t event;

  dht.temperature().getEvent(&event);
  Serial.println(event.temperature);

  dht.humidity().getEvent(&event);
  Serial.println(event.relative_humidity);

  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURA: ");

  if (!isnan(event.temperature) && !isnan(event.relative_humidity)) {
    displayTemperatureAndHumidity(event.temperature, event.relative_humidity);
  }
}

void displayTemperatureAndHumidity(int temperature, int humidity) {
  lcd.clear();

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
}

void triggerBuzzer(int temperature, int humidity) {
  if (temperature > MAX_TEMPERATURE || humidity < MIN_HUMIDITY) {
    for (int i = 0; i <= 3; i++) {
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
