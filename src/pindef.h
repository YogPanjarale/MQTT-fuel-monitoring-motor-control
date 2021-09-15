#include <Arduino.h>
#ifndef PINDEFS 
#define PINDEFS
/*
pins refrence https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
  Good Input Output pins 
    14,25,33,32
  Physical Pin Mappings
    relay1 -> 32
    relay2 -> 33
    Battery -> 34
  Virtual Pin Mappings 
    6  -> sensor1
    7  -> sensor2
    10 -> Battery
    11 -> relay1
    12 -> relay2
    13 -> relay3 
    .
    .
*/
const uint8_t RELAY1 = 32;
const uint8_t  RELAY2 =  33;

const uint8_t BATTERYREF  = 34;

#endif