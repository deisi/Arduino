ARDUINO = /usr/share/arduino
ARDUINO_DIR = $(ARDUINO)
ARDMK_VENDOR = archlinux-arduino
# The following lines are needed because arduino make cannot find libraries from
# inside libraries
ARDUINO_LIBS +=  $(notdir $(wildcard $(USER_LIB_PATH)/*))
ARDUINO_LIBS +=  SPI Wire SoftwareSerial

# Even though I use mostly Mini, nano seems to be correct
BOARD_TAG = pro
BOARD_SUB = 16MHzatmega328
#MONITOR_PORT = /dev/ttyUSB0

# Monitor commands
MONITOR_CMD = picocom
MONITOR_PARAMS = --imap lfcrlf


include $(ARDUINO)/Arduino.mk
