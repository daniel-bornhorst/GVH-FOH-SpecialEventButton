/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Bounce2.h>
#include "elapsedMillis.h"

// WiFi network name and password
// const char* networkName = "MrBrightSign";
// const char* networkPswd = "GrabFinkleStern3967%";

const char* networkName = "MW-IOT";
const char* networkPswd = "CrimpleLump52#";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "10.32.70.24";
const int udpPort = 9000;

// const char * udpAddress = "10.32.16.123";
// const int udpPort = 6543;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

bool buttonState = false;

const int builtInLed = LED_BUILTIN;
const int button = A0;

const int buttonLed = SDA;

const int buttonPin = 0;
const int PIN = 1;

elapsedMillis repeatTriggerTimer;
long unsigned int repeatTriggerTime = 15000; // 15 seconds

Bounce2::Button eventButton = Bounce2::Button();

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  delay(2);

  eventButton.attach(button, INPUT_PULLUP);
  eventButton.interval(50);
  eventButton.setPressedState(LOW);

  pinMode(builtInLed, OUTPUT);
  delay(100);
  digitalWrite(builtInLed, 0);

  pinMode(buttonLed, OUTPUT);
  digitalWrite(buttonLed, LOW);

  delay(5000);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

  repeatTriggerTimer = repeatTriggerTime; // allow for button push immediately after boot
  
  //Initialize Arcade buttonLed as an output
  pinMode(buttonLed, OUTPUT);

  //buttonLed Button has been pressed
  digitalWrite(PIN, HIGH);

}

void loop(){

  if (repeatTriggerTimer < repeatTriggerTime) {
    delay(10);
    return;
  }

  eventButton.update();

  if (eventButton.fell() && connected) {

    //eventTriggered();
    repeatTriggerTimer = 0;
    digitalWrite(builtInLed, HIGH); 
  }
  else if (eventButton.rose()) {
    digitalWrite(builtInLed, LOW); 
  }

  //Wait for 1 second
  delay(1);
  // the loop function runs over and over again forever

  //Read button state
  buttonState = digitalRead(buttonPin);

  //Blink buttonLed, button has not been pressed
  if (buttonState == LOW) {
  digitalWrite(buttonLed, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(5000);                      // wait for 5 seconds
  digitalWrite(buttonLed, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
  }

  //If button is pressed, buttonLed blinks fast for 15 seconds
  if (buttonState == HIGH) {
    digitalWrite(buttonLed, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(0500);                      // wait for 0.5 second
    digitalWrite(buttonLed, LOW);   // turn the LED off by making the voltage LOW
    delay(0500);                      // wait for 0.5 second
  }

  //Just need to get the flash to last for 15 seconds after the button has been released.


}

void eventTriggered() {
  Serial.println("Event Triggered");
  OSCMessage msg("/foh/zallway/specialtrigger");
  udp.beginPacket(udpAddress, udpPort);
  msg.send(udp); // send the bytes to the SLIP stream
  udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}
