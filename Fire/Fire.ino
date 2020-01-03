/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * This is an example that demonstrates how to report the battery level for a sensor
 * Instructions for measuring battery capacity on A0 are available here:
 * http://www.mysensors.org/build/battery
 * 
 */

// Enable debug prints to serial monitor
//#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <MySensors.h>
#include <SPI.h>


#define SKETCH_NAME "Fire"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

#define PRIMARY_BUTTON_PIN 2   // Arduino Digital I/O pin for button/reed switch

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

unsigned long SLEEP_TIME = 3600000;  // sleep time between reads (seconds * 1000 milliseconds)
#ifdef MY_DEBUG
SLEEP_TIME = 9000
#endif
int oldBatteryPcnt = 0;

MyMessage msg(MY_NODE_ID, V_TRIPPED);

void setup()  
{
   // use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
   analogReference(INTERNAL1V1);
#else
   analogReference(INTERNAL);
#endif

  // Setup the binary button
  pinMode(PRIMARY_BUTTON_PIN, INPUT);

  // Activate interall pull-ups
  digitalWrite(PRIMARY_BUTTON_PIN, HIGH);

}

void presentation() {
   // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(3, S_DOOR); 
}

void loop()
{
   // get the battery Voltage
   int sensorValue = analogRead(BATTERY_SENSE_PIN);
   #ifdef MY_DEBUG
   Serial.println('***Battery***')
   Serial.println(sensorValue);
   #endif
   
   // 1M, 470K divider across battery and using internal ADC ref of 1.1V
   // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
   // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
   // 3.44/1023 = Volts per bit = 0.003363075
   
   int batteryPcnt = sensorValue / 10;

   #ifdef MY_DEBUG
   float batteryV  = sensorValue * 0.003363075;
   Serial.print("Battery Voltage: ");
   Serial.print(batteryV);
   Serial.println(" V");

   Serial.print("Battery percent: ");
   Serial.print(batteryPcnt);
   Serial.println(" %");
   #endif

   if (oldBatteryPcnt != batteryPcnt) {
     // Power up radio after sleep
     sendBatteryLevel(batteryPcnt);
     oldBatteryPcnt = batteryPcnt;
   }

  // Read out Primary button pin
  uint8_t value;
  static uint8_t sentValue=2;
  
  // Short delay to allow buttons to properly settle
  sleep(5);
  
  value = digitalRead(PRIMARY_BUTTON_PIN);
  
  if (value != sentValue) {
     // Value has changed from last transmission, send the updated value
     send(msg.set(value==HIGH ? 1 : 0));
     sentValue = value;
  }
   
  // Sleep until something happens with the sensor or SLEEP TIME is over
  sleep(PRIMARY_BUTTON_PIN-2, CHANGE, SLEEP_TIME);
}
