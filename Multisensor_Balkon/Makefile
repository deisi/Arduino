ARDUINO = /usr/share/arduino
ARDUINO_DIR = $(ARDUINO)
ARDMK_VENDOR = archlinux-arduino
# The following lines are needed because arduino make cannot find libraries from
# inside libraries
ARDUINO_LIBS +=  $(notdir $(wildcard $(USER_LIB_PATH)/*))
ARDUINO_LIBS +=  SPI Wire SoftwareSerial


include $(ARDUINO)/Arduino.mk
