/*
  Universidade Federal do ABC 
  Engenharia Unificada II - 2023.3

  Project Name: 
    Seed Germination Box

  Project Description: 
    The software developed below aims to control an Arduino Uno board for the home automation of seed cultivation. 
    Initially, the project relies on temperature and relative humidity parameters as guidelines for activating seed irrigation.
  
  Code Author: 
    JENIFER SOUZA
  Code CoAuthor: 
    GUSTAVO HENRIQUE 

*/

/************************************************************************
************************* General Program Libraries *********************
*************************************************************************/

// Libraries used to communicate with the I2C module and the 20x4 LCD display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Library used to communicate with the RTCClock
#include <DS3231.h>

// Library used to communicate with the Servo Motor
#include <Servo.h>

// Library used to communicate with the DHT11 Sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

/************************************************************************
************************* General Program Settings **********************
*************************************************************************/

// DHT11 Settings
#define DHTTYPE DHT11
#define DHT11PIN 5

DHT_Unified dht(DHT11PIN, DHTTYPE);

// Servo Motor Settings
int MOTOR_PIN = 8;

Servo servoMotor;
bool hasServoNotRunned;

// 20x4 LCD Display Settings
LiquidCrystal_I2C lcd(0x27, 20, 4);

// RTCClock Settings
RTClib rtc;

//  Spray Trigger Settings
int TARGET_MINUTE = 5;
byte hours[6] = { 'x', 'x', 'x', 'x', 'x', 'x' };

int MAX_TEMPERATURE = 32;
int MIN_HUMIDITY = 40;


void setup() {
  servoMotor.attach(MOTOR_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  dht.begin();

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
    defineHoursToRun(temperature, humidity);
  }

  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  if (hour == 0 && minute == 0 && hours[0] != 'x') {
    resetHours();
  }

  triggerMotor(hour, minute);
}

void displayTemperatureAndHumidity(int temperature, int humidity) {
  /*
    Method responsible for displaying the measured temperature and 
    relative humidity values from the sensor on the display screen
  */

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

void resetHours() {
  /*
    Method responsible for clearing the data from the spray activation routine, 
    so that a new one can be created based on the temperature and relative humidity values
  */

  for (int i = 0; i < 6; i++) {
    hours[i] = 'x';
  }
}

void triggerMotor(int hour, int minute) {
  /*
    Method responsible for enabling the servo motor at the predefined hours and minutes only once each, 
    based on the parameters 'turnMotorOn' and 'hasServoNotRunned'
  */

  bool turnMotorOn = false;

  for (int i = 0; i < 6; i++) {
    if (hour == hours[i] && minute == TARGET_MINUTE) {
      turnMotorOn = true;
      break;
    }
  }

  // If it is not to activate the servo and the servo has already run before
  if (!turnMotorOn && !hasServoNotRunned) {
    hasServoNotRunned = true;
  }

  if (turnMotorOn && hasServoNotRunned) {
    moveServo(10);
    hasServoNotRunned = false;
  }
}

void primeSpray() {
  /*
    Method responsible for activating the motor upon system initialization, 
    allowing water to pass through the cannula and reach the spray outlet
  */

  moveServo(6);
}

void moveServo(int sprayQuantity) {
  /*
    Method responsible for triggering the spray behavior, 
    moving the servo between angles 0 and 180 for a certain number of times
  */

  for (int j = 0; j < sprayQuantity; j++) {
    servoMotor.write(0);
    delay(500);
    servoMotor.write(180);
    delay(500);
  }
}

void defineHoursToRun(int humidity, int temperature) {
  /*
    Method responsible for defining the quantity and times of spray activation, 
    all based on the temperature and relative humidity measurements at 00:00 of the day
  */

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
