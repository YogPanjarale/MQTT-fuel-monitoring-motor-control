#include <Arduino.h>

#include "max6675.h"

int thermoDO = 25;
int thermoCS = 26;
int thermoCLK = 27;
int waterSensor = 14;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup()
{
  Serial.begin(9600);

  pinMode(waterSensor, INPUT);
  // wait for MAX chip to stabilize
  delay(500);
}

void loop()
{
  // basic readout test, just print the current temp

  Serial.print("C = ");
  Serial.println(thermocouple.readCelsius());
  Serial.print("F = ");
  Serial.println(thermocouple.readFahrenheit());

  Serial.printf("Water sensor : %s\n", digitalRead(waterSensor) == HIGH ? "Off" : "On");
  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(1000);
}
