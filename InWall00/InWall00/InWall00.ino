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
 * Interrupt driven binary switch example with dual interrupts
 * Author: Patrick 'Anticimex' Fallberg
 * Connect one button or door/window reed switch between
 * digitial I/O pin 3 (BUTTON_PIN below) and GND and the other
 * one in similar fashion on digital I/O pin 2.
 * This example is designed to fit Arduino Nano/Pro Mini
 *
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

// Enable Signing
#define MY_SIGNING_REQUEST_SIGNATURES

#define MY_NODE_ID 10

#include <MySensors.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SKETCH_NAME "In Wall Sensor 00"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

// MySensor ChildID of the Onewire sensor
#define TEMP_CHILD_ID 1
MyMessage msg_temp(TEMP_CHILD_ID, V_TEMP);

// Temperature offset
float TEMP_OFFSET = -4;
int LAST_TEMP;

// Pin of the Onweire sensor
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);

// Pin of the Relay
#define RELAY_PIN 3
#define RELAY_STATUS_ID 2
MyMessage msg_r(RELAY_STATUS_ID, V_STATUS);
bool stat = true;

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

// Must send initial values.
bool initialValueSent = false;

void setup()
{
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  //insideThermometer = { 0x28, 0x5A, 0x9E, 0x4C, 0x6, 0x0, 0x0, 0xC9 };
  //28 5A 9E 4C 06 00 00 C9
  // Search for device bus adress
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  //sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();

  // Setting Pin to Output mODE
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(RELAY_PIN, OUTPUT);
}

void presentation()
{
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  present(TEMP_CHILD_ID, S_TEMP);
  present(RELAY_STATUS_ID, S_BINARY);

}

void receive(const MyMessage &message)
{
  if (message.type == V_STATUS){
    // Initial Value Recevied.
    if (!initialValueSent) {
      Serial.println("Receiving initial value from controller");
      initialValueSent = true;
    }
    // Retrieve status from incomming message
    stat = atoi(message.data);

    Serial.print("Setting Relay pin to: ");
    Serial.print(stat);
    Serial.print("\n");
    
    digitalWrite(RELAY_PIN, stat);
    send(msg_r.set(stat));
  }
}

void loop()
{
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    send(msg_r.set(stat));
    Serial.println("Requesting initial value from controller");
    request(RELAY_STATUS_ID, V_STATUS);   
    }

  
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  int tempC = round(sensors.getTempC(insideThermometer) + TEMP_OFFSET);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print("\n");
  if (tempC != LAST_TEMP){
     send(msg_temp.set(tempC));
     LAST_TEMP = tempC;
    }
  wait(10000);
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
