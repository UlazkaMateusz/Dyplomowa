FILENAME?=main

c_file:
	xsltproc --novalid parser.xslt ${FILENAME}.xml > ${FILENAME}.c

c_clear: 
	rm -f ${FILENAME}.c

flash:
	avrdude -p attiny13 -c usbasp -U flash:w:${FILENAME}.hex:i -P usb

bin_hex_clear:
	rm -f *.bin *.hex

build:
	avr-gcc -std=c99 -Wall -g -Os -mmcu=attiny13 -o ${FILENAME}.bin ${FILENAME}.c
	avr-objcopy -j .text -j .data -O ihex ${FILENAME}.bin ${FILENAME}.hex

c_file_with_clear: c_clear c_file

clear: c_clear bin_hex_clear

full: c_file build flash c_clear bin_hex_clear

size:
	avr-size ${FILENAME}.hex

build_and_size: build size
