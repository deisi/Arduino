/*
* The MySensors Arduino library handles the wireless radio link and protocol
* between your home built sensors/actuators and HA controller of choice.
* The sensors forms a self healing radio network with optional repeaters. Each
* repeater and gateway builds a routing tables in EEPROM which keeps track of the
* network topology allowing messages to be routed to nodes.
*
* Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
* Copyright (C) 2013-2017 Sensnology AB
* Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*/

/**************************
Template

This sketch can be used as a template since containing the most relevant MySensors library configuration settings,
NodeManager's settings, all its the supported sensors commented out and a sketch structure fully functional to operate with
NodeManager. Just uncomment the settings you need and the sensors you want to add and configure the sensors in before()
*/

/**********************************
 * MySensors node configuration
 */

/*
 *  Changelog.
 *  1.0:
 *    flashed on 1.1.2020
 *    - Debugging on
 *    - bmp180 temp ans pressure included but broken
 *    - light sensor on pin A0
 *    - dht22 temp and hum on pin 6
 *    - sds011 feinstaub on pin 2, 3
 *  1.1:
 *    - changed name to Multisensor_Balkon
 *    - use MY_RADIO_RF24 instead if MY_RADIO_NRF24
 *  1.2:
 *    - Use 10 min Report time for SDS
 *    - Use 1 min report time but minimal differences for sending changes
 *  2.0:
 *    - Flashed on 4.1.20
 *    - No Debugging
 *    - Added Splashscreen
 *  3.0:
 *    flashed 5.1.20
 *    - Add bme280
 *    - Configured individual report deltas
 *    - OTA_CONFIG must be off. Over 85% usage, the node doesn't work
 *  3.1:
 *    flashed 15.1.20. Changed arduino
 *    - Turned on debugging
 *  3.2:
 *    flashed 15.1.20. Changed arduino
 *    - Turned debugging off
 *  3.3:
 *    flashed 18.1.20
 *    - Turn of conditional reports. I think they just don't work
 *    - Report every 10 min and brightness every 2
 *    - Turned on OTA config
 *  3.4
 *    - fix time of helligkeits sensor
 */

// General settings
#define SKETCH_NAME "Multisensor_Balkon"
#define SKETCH_VERSION "3.4"
//#define MY_DEBUG
//#define MY_NODE_ID 99

// NRF24 radio settings
#define MY_RADIO_RF24
//#define MY_RF24_ENABLE_ENCRYPTION
//#define MY_RF24_CHANNEL 125
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
//#define MY_DEBUG_VERBOSE_RF24
//#define MY_RF24_DATARATE RF24_250KBPS

// Message signing settings
//#define MY_SIGNING_SOFT
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN 7
//#define MY_SIGNING_REQUEST_SIGNATURES
//#define MY_SIGNING_ATSHA204
//#define MY_SIGNING_ATSHA204_PIN 4
//#define MY_SIGNING_REQUEST_SIGNATURES

// OTA Firmware update settings
//#define MY_OTA_FIRMWARE_FEATURE
//#define OTA_WAIT_PERIOD 300
//#define FIRMWARE_MAX_REQUESTS 2
//#define MY_OTA_RETRY 2

// OTA debug output
//#define MY_DEBUG_OTA (0)
//#define MY_OTA_LOG_SENDER_FEATURE
//#define MY_OTA_LOG_RECEIVER_FEATURE
//#define MY_DEBUG_OTA_DISABLE_ACK

// Advanced settings
#define MY_BAUD_RATE 9600
//#define MY_SMART_SLEEP_WAIT_DURATION_MS 500
//#define MY_SPLASH_SCREEN_DISABLED
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE
//#define MY_SIGNAL_REPORT_ENABLED

/***********************************
 * NodeManager configuration
 */

#define NODEMANAGER_DEBUG OFF
#define NODEMANAGER_INTERRUPTS OFF
#define NODEMANAGER_SLEEP OFF
#define NODEMANAGER_RECEIVE ON
#define NODEMANAGER_DEBUG_VERBOSE OFF
#define NODEMANAGER_POWER_MANAGER OFF
#define NODEMANAGER_CONDITIONAL_REPORT OFF
#define NODEMANAGER_EEPROM OFF
#define NODEMANAGER_TIME OFF
#define NODEMANAGER_RTC OFF
#define NODEMANAGER_SD OFF
#define NODEMANAGER_HOOKING OFF
#define NODEMANAGER_OTA_CONFIGURATION ON
#define NODEMANAGER_SERIAL_INPUT OFF

// import NodeManager library (a nodeManager object will be then made available)
#include <MySensors_NodeManager.h>


/***********************************
 * Add your sensors
 */

#include <sensors/SensorSDS011.h>
SensorSDS011 sds011(2,3);

#include <sensors/SensorBME280.h>
SensorBME280 bme280;

#include <sensors/SensorBH1750.h>
SensorBH1750 bh1750;

/***********************************
 * Main Sketch
 */

// before
void before() {

  /***********************************
   * Configure your sensors
   */
  sds011.children.get(1)->setDescription("FEINSTAUB 10");
  sds011.children.get(2)->setDescription("FEINSTAUB 2.5");

  bme280.children.get(1)->setDescription("TEMPERATUR");
  bme280.children.get(2)->setDescription("LUFTFEUCHTIGKEIT");
  bme280.children.get(3)->setDescription("LUFTDRUCK");
  bme280.children.get(4)->setDescription("VORHERSAGE");

  bh1750.setReportIntervalMinutes(2);
  bh1750.children.get()->setDescription("HELLIGKEIT");

  // EXAMPLES:
  // report measures of every attached sensors every 10 seconds
  //nodeManager.setReportIntervalSeconds(10);
  // report measures of every attached sensors every 10 minutes
  nodeManager.setReportIntervalMinutes(10);
  // set the node to sleep in 30 seconds cycles
  //nodeManager.setSleepSeconds(30);
  // set the node to sleep in 5 minutes cycles
  //nodeManager.setSleepMinutes(5);
  // report battery level every 10 minutes
  //battery.setReportIntervalMinutes(10);
  // set an offset to -1 to a thermistor sensor
  //thermistor.setOffset(-1);
  // change the id of a the first child of a sht21 sensor
  //sht21.children.get(1)->setChildId(5);
  // report only when the analog value is above 40%
  //analog.children.get(1)->setMinThreshold(40);
  // power all the nodes through dedicated pins
  //nodeManager.setPowerManager(power);

  // call NodeManager before routine
  nodeManager.before();
}

// presentation
void presentation() {
  // call NodeManager presentation routine
  nodeManager.presentation();
}

// setup
void setup() {
  // call NodeManager setup routine
  nodeManager.setup();
}

// loop
void loop() {
  // call NodeManager loop routine
  nodeManager.loop();
}

#if NODEMANAGER_RECEIVE == ON
// receive
void receive(const MyMessage &message) {
  // call NodeManager receive routine
  nodeManager.receive(message);
}
#endif

#if NODEMANAGER_TIME == ON
// receiveTime
void receiveTime(unsigned long ts) {
  // call NodeManager receiveTime routine
  nodeManager.receiveTime(ts);
}
#endif
