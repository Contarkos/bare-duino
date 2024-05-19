#!/bin/bash
# Update the firmware in the Arduino Mini

_BAUDRATE=115200

if [ -n $1 ]; then
	_PORT_COM=$1
else
	_PORT_COM="S3"
fi

if [ -n $2 ]; then
	_FILE_FLASH=$2
else
	_FILE_FLASH=/mnt/c/partage_vm/arduino.hex
fi

#avrdude -F -V -c arduino -p ATMEGA328p -P /dev/tty${_PORT_COM} -b ${_BAUDRATE} -U flash:w:${_FILE_FLASH}
avrdude -v -C /etc/avrdude.conf -c arduino -p atmega328p -P /dev/tty${_PORT_COM} -D -b ${_BAUDRATE} -U flash:w:${_FILE_FLASH}:i

