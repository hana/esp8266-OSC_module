/*
   GPIO over OSC module v1.0
   made by Hana Taki. @fataris
   Licence : CC BY-NC-SA 4.0
*/


extern "C" {
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Metro.h>
#include "types.h"

#define SAMPLING_RATE 25
#define ACTIVE_SIGNAL_INTERVAL 1000
#define GPIO_MODE_INPUT 0
#define GPIO_MODE_OUTPUT 1

static const int MODULE_ID = 1; //If you want to use more than one module, set id on each module.

char ssid[] = "YOURSSID";          // your network SSID (name)
char pass[] = "YOURPASS";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
IPAddress hostAddress(224, 0, 0, 1);     // remote IP of your computer
const unsigned int hostPort = 56789;          // remote port to receive OSC
const unsigned int receivePort = 54321;        // local port to listen to OSC packets

//To Receive
OSCErrorCode error;

//GPIO MODE 0 : Input, 1 : Output
boolean IO12_mode, IO13_mode, IO14_mode;

IO_Mode_t modes;
Input_Values_t inputs;


//Metro
Metro readMetro = Metro(SAMPLING_RATE);
Metro activeMetro = Metro(ACTIVE_SIGNAL_INTERVAL);

void sendMessage(OSCMessage &msg) {
  Udp.beginPacket(hostAddress, hostPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void digitalOut(OSCMessage &msg) {
  int id = msg.getInt(0);
  if (id == MODULE_ID) {
    float ch = msg.getInt(1);
    int val = msg.getFloat(2) * 1023;
    analogWrite(ch, val);
  }
}

void sendInput(int ch, int val) {
  OSCMessage mes("/digital_in");
  mes.add(MODULE_ID);
  mes.add(ch);
  mes.add(val);
  sendMessage(mes);
}

void sendDigitalIn() {
  int val = 0;
  if (modes.io12 == INPUT) {
    val = digitalRead(12);
    if (val != inputs.io12) {
      sendInput(12, val);
      inputs.io12 = val;
    }
  }
  if (modes.io13 == INPUT) {
    val = digitalRead(13);
    if (val != inputs.io13) {
      sendInput(13, val);
      inputs.io13 = val;
    }
  }
  if (modes.io14 == INPUT) {
    val = digitalRead(14);
    if (val != inputs.io14) {
      sendInput(14, val);
      inputs.io14 = val;
    }
  }
  if (modes.io16 == INPUT) {
    val = digitalRead(16);
    if (val != inputs.io16) {
      sendInput(16, val);
      inputs.io16 = val;
    }
  }
}

void sendAnalogIn() {
  float val = system_adc_read();
  val /= 1024.0;  //rescale into 0.0 ~ 1.0
  OSCMessage mes("/analog_in");
  mes.add(MODULE_ID);
  mes.add(val);
  sendMessage(mes);
}

void setGpioMode(OSCMessage &msg) {
  int ID = msg.getInt(0);

  if (ID == MODULE_ID) {
    int ch = msg.getInt(1);
    int gpioMode = msg.getInt(2); //0 -- input. 1--output
    switch (ch) {
      case 12:
        modes.io12 = gpioMode;
        pinMode(12, modes.io12);
        break;
      case 13:
        modes.io13 = gpioMode;
        pinMode(13, modes.io13);
        break;
      case 14:
        modes.io14 = gpioMode;
        pinMode(14, modes.io14);
        break;
      case 16:
        modes.io16 = gpioMode;
        pinMode(16, modes.io16);
        break;
      default:
        break;
    }
  }
}

void setHostAddress(OSCMessage &msg) {
  int adr0 = msg.getInt(0);
  int adr1 = msg.getInt(1);
  int adr2 = msg.getInt(2);
  int adr3 = msg.getInt(3);

  hostAddress[0] = adr0;
  hostAddress[1] = adr1;
  hostAddress[2] = adr2;
  hostAddress[3] = adr3;
}

void sendActiveSignal() {
  OSCMessage mes("/active");
  mes.add(MODULE_ID);
  mes.add(1);
  sendMessage(mes);
}

void setupWiFi() {
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setupUdp() {
  Udp.begin(receivePort);
}

void initGpio() {
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  inputs.io12 = digitalRead(12);
  inputs.io13 = digitalRead(13);
  inputs.io14 = digitalRead(14);
  inputs.io16 = digitalRead(16);

  modes.io12 = INPUT;
  pinMode(12, modes.io12);
  modes.io13 = INPUT;
  pinMode(13, modes.io13);
  modes.io14 = INPUT;
  pinMode(14, modes.io14);
  modes.io16 = INPUT;
  pinMode(16, modes.io16);

}

void updateOsc() {
  //Check receive
  OSCMessage msgIn;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msgIn.fill(Udp.read());
    }
    if (msgIn.hasError()) {
      error = msgIn.getError();
    } else {
      msgIn.dispatch("/gpio_mode", setGpioMode);
      msgIn.dispatch("/digital_out", digitalOut);
      msgIn.dispatch("/set_address", setHostAddress);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  setupUdp();

  initGpio();
}

void loop() {
  //Read OSC message
  updateOsc();

  //Send GPIO values
  if (readMetro.check()) {
    sendDigitalIn();
    sendAnalogIn();
  }

  if (activeMetro.check()) {
    sendActiveSignal();
  }
}




