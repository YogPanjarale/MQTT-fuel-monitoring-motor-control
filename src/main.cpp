#define SIM800L_AXP192_VERSION_20200327
#include <Arduino.h>
#include <pindef.h>
#include <SimpleTimer.h>
#include <details.h>
#include <utilities.h>
#include <converters.h>
// Select your modem:
#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN ""
// Your GPRS credentials, if any
const char apn[] = "www";

// MQTT details
const char *topicLed = "/led";
const char *topicInit = "/init";
const char *topicPing = "/ping";
const char *topicLedStatus = "/ledStatus";
const char *topicBattery = "/battery";
const char *topicRelay1 = "/relay1";
const char *topicRelay2 = "/relay2";
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

SimpleTimer pingTimer;
void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, len);
    SerialMon.println();
    // Only proceed if incoming message's topic matches
    if (String(topic) == withTopic(topicLed))
    {
        ledStatus = !ledStatus;
        digitalWrite(13, ledStatus);
        mqtt.publish(withTopic(topicLedStatus), ledStatus ? "1" : "0");
    }
    if (String(topic) == withTopic(topicRelay1)){
        String value = (char* )payload;
        value = value.charAt(0);
        SerialMon.print("Relay 1: ");
        SerialMon.println(value);
        if (value == "1"||value=="0"){
            digitalWrite(RELAY1,value=="0"?LOW:HIGH);
        }
    }
    if (String(topic) == withTopic(topicRelay2)){
        String value = (char* )payload;
        value = value.charAt(0);
        SerialMon.print("Relay 2: ");
        SerialMon.println(value);
        if (value == "1"||value=="0"){
            digitalWrite(RELAY2,value=="0"?LOW:HIGH);
        }
    }
}
boolean mqttConnect()
{
    SerialMon.print("Connecting to ");
    SerialMon.print(MQTT_BROKER);
    // Connect to MQTT Broker
    UserDetails details = parseUserDetails(AUTH_TOKEN);
    char *user = (char *)details.username.c_str();
    char *pass = (char *)details.password.c_str();
    Serial.printf("\nLogging In with username: %s and password: %s", user, pass);
    boolean status = mqtt.connect("TTGO-TCall", user, pass);
    // mqtt.connect("username",);
    if (status == false)
    {
        SerialMon.println(" fail");
        return false;
    }
    SerialMon.println(" success");
    mqtt.publish(withTopic(topicInit), "Online");
    
    mqtt.subscribe(withTopic(topicRelay1));
    mqtt.subscribe(withTopic(topicRelay2));
    // mqtt.unsubscribe(withTopic(topicPing));
    return mqtt.connected();
}
float voltage=0.0;
float getVoltage(){
  int read = analogRead(BATTERYREF);
  float R1=5.37;
  float R2 = 2.02;
  float voltage = read * (3.6/4095) * ((R1 + R2)/R2);
  return voltage;
}
void updateVoltage(){
  // analogReadResolution(16);
  voltage=0.0;
  float times = 100.0;
  for (size_t i = 0; i < times; i++)
  {
    float read=getVoltage();
    voltage=read+voltage;
      // SerialMon.printf("Read %u: %f \n",i,read);
  }
  //finding the volatge in float with many decimals
  voltage = voltage/times;
  //rounding it to 1 decimal
  voltage= (float)((int)(voltage*100))/100;
  SerialMon.printf("Battery Voltage computed : %f \n",voltage);
  String str = String(voltage);
  mqtt.publish(withTopic(topicBattery), str.c_str());
//   Blynk.virtualWrite(V10,str);
  /*
  +ve input
  R1 = 6.8k
  output->
  R2 = 2.2k
  -ve base
  Vm =  input voltage
  V1 = Vm * (R2/(R1+R2))
  */
}

int p1 = 0, p2 = 0, p3 = 0;
void ping()
{
    String val = String(p1);
    if (p2 > 0)
    {
        val = String(p2) + "," + val;
    }
    if (p3 > 0)
    {
        val = String(p3) + "," + val;
    }
    mqtt.publish(withTopic(topicPing), val.c_str());
    p1+=5;
    if (p1 >= 1000)
    {
        p1 = 0;
        p2+=5;
    }
    if (p2 >= 1000)
    {
        p2 = 0;
        p3+=5;
    }
}
void setupGPRS()
{
    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    modem.restart();
    // modem.init();
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);
#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }
#endif
    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");
    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
    }
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, "", ""))
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");
    if (modem.isGprsConnected())
    {
        SerialMon.println("GPRS connected");
    }
}
void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);
    //setting pin mappings
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(BATTERYREF, INPUT);
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    delay(10);
    setupModem();
    SerialMon.println("Wait...");
    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(6000);
    setupGPRS();
    // MQTT Broker setup
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);
    //timers
    pingTimer.setInterval(5000, ping);
    pingTimer.setInterval(30000, updateVoltage);
}

void loop()
{
    if (!mqtt.connected())
    {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L)
        {
            lastReconnectAttempt = t;
            if (mqttConnect())
            {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
        return;
    }
    mqtt.loop();
    pingTimer.run();
}