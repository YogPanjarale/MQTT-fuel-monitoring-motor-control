#include <Arduino.h>

#include "max6675.h"

int thermoDO = 25;
int thermoCS = 26;
int thermoCLK = 27;
int waterSensor = 32;
int rpmSensor = 14;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int delay1()
{
  //unsigned int long k;
  int i, j;
  unsigned int count = 0;
  for (i = 0; i < 1000; i++)
  {
    for (j = 0; j < 1000; j++)
    {
      if (digitalRead(rpmSensor))
      {
        count++;
        while (digitalRead(rpmSensor))
          ;
      }
    }
  }
  return count;
}
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
unsigned int time=0,RPM=0;


  Serial.print("C = ");
  Serial.println(thermocouple.readCelsius());
  Serial.print("F = ");
  Serial.println(thermocouple.readFahrenheit());
  // Read water sensor value
  Serial.printf("Water sensor : %s\n", digitalRead(waterSensor) == HIGH ? "Off" : "On");
time=delay1();

RPM=(time*12)/3;


  Serial.printf("RPM sensor : %d %d\n", time,RPM);
  Serial.println(digitalRead(rpmSensor));
  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(1000);
}
