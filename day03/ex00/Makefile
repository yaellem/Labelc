all:	hex flash 

flash:
	avrdude -b 115200 -p atmega328p -c arduino -U flash:w:main.hex:i -F -P /dev/ttyUSB*
hex:
	avr-gcc -mmcu=atmega328p -Os -D F_CPU=16000000 main.c -o main.bin
	avr-objcopy -O ihex main.bin main.hex
clean: 
	rm main.bin main.hex	

screen:
	    screen -S toto /dev/ttyUSB* 115200

quit_screen:
	    screen -Xs toto quit
