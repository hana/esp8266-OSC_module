# esp8266-OSC_module
![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)　　

## Overview
Controll all GPIO inputs/outputs through OSC.

## Description
This code enables you to controll all GPIO through OSC. For example, you can blink LED with Max/MSP. You also can use this Max Patch ( https://github.com/fataris/ESP8266-OSC_Basic_Controller ) to control the module.

## Requirement
- Metro (Arduino Playground)

## Usage
Each message requires __MODULE_ID__ to identify the target module.


### 1. Setup
Set your WiFi's SSID and passphrase

    char ssid[] = "SSID";          // your network SSID (name)  
    char pass[] = "PASS";                    // your network password

Set host Address and port

    IPAddress hostAddress(224, 0, 0, 1);     // remote IP of your computer
    const unsigned int hostPort = 56789;          // remote port to receive OSC

Set incoming port

    const unsigned int receivePort = 54321;        // local port to listen to OSC packets

### 2. Sending OSC message to ESP  

    /gpio_mode "(int)MODULE_ID" "(int)GPIO_ID" "(bool)IN/OUT"

* This messages changes pinMode()
* 0 = Input, 1 = Output.  
* GPIO_IDs :: 12, 13, 14  


    /digital_out "(int)MODULE_ID" "(int)GPIO_ID" "(float)val"

* This message sets value of DigitaiWrite()  
* val : 0.0 ~ 1.0, which will be 0.0V ~ 3.3V  

### 3. Receiving OSC message from ESP  

    /analog_in "(int)MODULE_ID" "(float)val"
* Analog input value comes with this message
* Value range : 0.0 ~ 1.0


    /digital_in "(int)MODULE_ID" "(int)val"

* Digital input value comes with this message
* Value : 0, 1  


    /active "(int)MODULE_ID" 1

* This message comes every second to notice the module is acitve.


## LICENSE
<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
