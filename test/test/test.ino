// Include your libraries at the top
#include <SPI.h>
#include <MySensors.h>


void before()
{
    // Optional method - for initialisations that needs to take place before MySensors transport has been setup (eg: SPI devices).
}

void setup()
{
    // Called once at startup, usually used to initialize sensors.
}

void presentation()  
{ 
  sendSketchInfo("Name of my sensor node", "1.0");
  present(CHILD_ID, S_WHATEVER);
}

void loop()
{
    // This will be called continuously after setup.
}
