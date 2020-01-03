
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
 * Fire is the LED stripe arduino
 */

#define MY_DEBUG 
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h> 

#define SN "RGB-LED"
#define SV "0.1"

#define CHILD_ID 0
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6
#define FADE_DELAY 10  // Delay in ms for each percentage fade up/down (10ms = 1s full-range dim)
#define LIGHT_OFF 0
#define LIGHT_ON 1

// V_RGB values first two byte blue, then two green, then to red and two white.
//unsigned long wrgb = 4294967295; // Default all on 
unsigned long rgb = 16777215; // Default all on 
int16_t last_state = LIGHT_ON;
String last_rgb_string = "ffffff";
int brightness_level = 100;  // brightness level in %...

MyMessage rgbMsg(CHILD_ID, V_RGB);
MyMessage lightMsg(CHILD_ID, V_STATUS);
MyMessage brightnessMsg(CHILD_ID, V_PERCENTAGE);

void presentation(){
  sendSketchInfo(SN, SV, true);
  present(CHILD_ID, S_RGB_LIGHT);

  //Do I need them here?
  //send_rgb_message();
  //send_status_message();
  //send_dimmer_message();
}

void setup() {
  // Pull the gateways current rgb level.
  request(CHILD_ID, V_RGB);
  //request(MY_NODE_ID, V_PERCENTAGE);
  
  // Prepare pins for output mode.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(115200);


  // Do I need them here?
  //send_rgb_message();
  //send_status_message();
  //send_dimmer_message();
}


void loop() {
  //In MySensors2.x, first message must come from within loop()
  static bool first_message_sent = false;
  if ( first_message_sent == false ) {
    Serial.println( "Sending initial state..." );
    send_rgb_message();
    send_status_message();
    send_dimmer_message();
    first_message_sent = true;
  }
}

void receive(const MyMessage &message) {
  if (message.type == V_RGB)  {
    
    Serial.println("V_RGB Command recieved...");
    //  Retrieve the rgb values from the incoming request message
    String hexstring = message.getString();
    
    #ifdef MY_DEBUG
    Serial.print("Hex string is: ");
    Serial.print(hexstring);
    Serial.print("\n");
    #endif
    
    set_rgb_values(hexstring);
    get_brightness_from_rgb();
    send_rgb_message();
    send_dimmer_message();
  }

  else if ( message.type == V_STATUS ) {
    Serial.println( "V_STATUS command received..." );

    last_state = message.getInt();
    if (( last_state < 0 ) || ( last_state > 1 )) {
      Serial.println( "V_STATUS data invalid (should be 0/1)" );
      return;
    }
    
    // If light was turned off using the brighness, we need to
    // tune the brightness back up again.
    if ( last_state == LIGHT_ON && brightness_level == 0) {
      brightness_level=100;
    }
    #ifdef MY_DEBUG
    Serial.print("brightness_level is: ");
    Serial.print(brightness_level);
    Serial.print("\n");
    #endif

    // Turn off the LEDS:
    toggle_leds();
    
    //Update constroller status
    send_status_message();
    send_dimmer_message();
    
  } else if ( message.type == V_PERCENTAGE ) {
    Serial.println( "V_PERCENTAGE command received..." );
    // Isn't map better here
    brightness_level = constrain( message.getInt(), 0, 100 );
    #ifdef MY_DEBUG
    //Serial.print("brightness_level: %d\n", brightness_level);
    #endif
    if ( brightness_level == 0 ) {
      last_state = LIGHT_OFF;     
    } else {
      last_state = LIGHT_ON;
    }

    // Use brighness to dim RGB values uniformly
    dim_rgb_with_brightness();
    
    //Update constroller with dimmer value & status
    send_status_message(); 
    send_dimmer_message();
  } else {
    Serial.println("Got unknown message type: ");
    Serial.println(message.type);
  }
}

void get_brightness_from_rgb(){
  // use the biggest rgb value as common brightness

  unsigned int color;
  int brightness = 0;
  int brightness_candidate = 0;

  for (int i; i<3; i++){
    color = rgb >> 8*i &0xFF;
    brightness = map(color, 0, 255, 0, 100);

    // make shure we get the biggest possibe
    if (brightness > brightness_candidate){
      brightness_candidate = brightness;
    }
  }
  // set the actual brighness level
  brightness_level = brightness_candidate;
}

void dim_rgb_with_brightness(){
  // Use the brighness level to dim the rgbs
    unsigned int r = rgb >> 16 & 0xFF;
    unsigned int g = rgb >> 8 & 0xFF;
    unsigned int b = rgb & 0xFF; 

    // by keeping proportions of rgb the same, we only dim
    // and do not change the color.
    r = r * brightness_level/100;
    g = g * brightness_level/100;
    b = b * brightness_level/100;

    #ifdef MY_DEBUG
    Serial.print("Scaling rgb values with brightness: ");
    Serial.print(brightness_level);
    Serial.println("");
    #endif

    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}

void toggle_leds(){
  if (last_state == LIGHT_OFF){
    last_rgb_string = String(rgb, HEX);
    set_rgb_values("000000");
  }
  else {
    set_rgb_values(last_rgb_string);
  }
}

void set_rgb_values(String hexstring){
      rgb = strtol( &hexstring[0], NULL, 16);

    //unsigned int w = wrgb >>24 & 0xFF;
    unsigned int r = rgb >> 16 & 0xFF;
    unsigned int g = rgb >> 8 & 0xFF;
    unsigned int b = rgb & 0xFF; 

    #ifdef MY_DEBUG
    Serial.print("Changing color to: ");
    Serial.print(rgb);
    Serial.print("\n");
    Serial.print("Red : ");
    Serial.print(r);
    Serial.print("\nGreen: ");
    Serial.print(g);
    Serial.print("\nBlue: ");
    Serial.print(b);
    Serial.print("\n");
    #endif
    
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}

void send_rgb_message()
{
  // need to make shure, that a 3 bit char array is send
  // to HA if one only wants an S_RGB_LIGHT and not
  // S_RGBW_LIGHT.
  String msg_string = String(rgb, HEX);
  char msg[7];
  msg_string.toCharArray(msg, 7);
  #ifdef MY_DEBUG
  Serial.print("Sending msg_string: ");
  Serial.print(msg_string);
  Serial.print("\n");
  #endif
  send( rgbMsg.set(msg) );
}

void send_status_message()
{
    send( lightMsg.set( last_state ) );
}

void send_dimmer_message()
{
  send( brightnessMsg.set( brightness_level ) );
}
