ARDUINO = /usr/share/arduino
ARDUINO_DIR = $(ARDUINO)
ARDMK_VENDOR = archlinux-arduino
# The following lines are needed because arduino make cannot find libraries from
# inside libraries
ARDUINO_LIBS +=  $(notdir $(wildcard $(USER_LIB_PATH)/*))
ARDUINO_LIBS +=  SPI Wire SoftwareSerial

# Even though I use mostly Mini, nano seems to be correct
BOARD_TAG = nano
BOARD_SUB = atmega328old
#MONITOR_PORT = /dev/ttyUSB0


include $(ARDUINO)/Arduino.mk
