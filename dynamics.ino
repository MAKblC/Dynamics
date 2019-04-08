/*
  Пример работы с цифровым датчиком тока и напряжения INA226
  Created by Rostislav Varzar
*/
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <INA226.h>

int strong = 0;

#include <Adafruit_PWMServoDriver.h>
INA226 ina226;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x70);

char ssid[] = "MGBot";                            // Логин Wi-Fi  // Wi-Fi login
char pass[] = "Terminator812";                    // Пароль от Wi-Fi // Wi-Fi password
char auth[] = "f6e62ba51f3d4ce2b2ab1d65fd45d1cc"; // Токен // Authorization token
IPAddress blynk_ip(139, 59, 206, 133);            // конфигурация блинка // Blynk configuration

#define UPDATE_TIMER 1000
BlynkTimer timer_update;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  // Инициализация датчика
  Blynk.begin(auth, ssid, pass, blynk_ip, 8442);
  Wire.begin();
  pwm.begin();
  // Частота (Гц)
  pwm.setPWMFreq(100);
  // Все порты выключены
  pwm.setPWM(8, 0, 4096);
  pwm.setPWM(9, 0, 4096);
  pwm.setPWM(10, 0, 4096);
  pwm.setPWM(11, 0, 4096);

  ina226.begin(0x40);
  ina226.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina226.calibrate(0.002, 10);

  timer_update.setInterval(UPDATE_TIMER, readSendData);
}
BLYNK_WRITE(V3)
{
  strong = param.asInt();
   Serial.println(strong);
}
BLYNK_WRITE(V2)
{
  int x = param[0].asInt();
  int y = param[1].asInt();
  Serial.println(x);
  Serial.println(y);
  if (y == 0) {
    motorA_setpower(strong, false);
    motorB_setpower(strong, true);
  }
  if (y == 100) {
    motorA_setpower(strong, true);
    motorB_setpower(strong, false);
  }
  if (x == 0) {
    motorA_setpower(strong, false);
    motorB_setpower(strong, false);
  }
 if (x == 100) {
    motorA_setpower(strong, true);
    motorB_setpower(strong, true);
  }
  if (x == 50 and y == 50) {
    motorA_setpower(0, true);
    motorB_setpower(0, true);
  }
}
void readSendData() {
  Blynk.virtualWrite(V1, ina226.readBusPower(), 5); delay(25);
  Blynk.virtualWrite(V0, ina226.readBusVoltage(), 5); delay(25);
}
void loop() {
  Blynk.run();
  timer_update.run();
}
// Мощность мотора "A" от -100% до +100% (от знака зависит направление вращения)
void motorA_setpower(float pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  int pwmvalue = fabs(pwr) * 40.95;
  if (pwr < 0)
  {
    pwm.setPWM(10, 0, 4096);
    pwm.setPWM(11, 0, pwmvalue);
  }
  else
  {
    pwm.setPWM(11, 0, 4096);
    pwm.setPWM(10, 0, pwmvalue);
  }
}

// Мощность мотора "B" от -100% до +100% (от знака зависит направление вращения)
void motorB_setpower(float pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  int pwmvalue = fabs(pwr) * 40.95;
  if (pwr < 0)
  {
    pwm.setPWM(8, 0, 4096);
    pwm.setPWM(9, 0, pwmvalue);
  }
  else
  {
    pwm.setPWM(9, 0, 4096);
    pwm.setPWM(8, 0, pwmvalue);
  }
}
