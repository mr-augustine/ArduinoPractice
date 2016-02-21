# Using an Arduino Uno
# To invoke this makefile while another makefile exists in the same directory:
#  make -f MyMakefile all
F_CPU = 16000000L
MCU = atmega328p

ARD_PORT = /dev/ttyACM0

TARGET = main

OBJ_DIR = obj

OBJ = obj/main.o \
      obj/onboard_logger.o	

CFLAGS = -std=gnu99 -Os -Werror \
		 -mmcu=$(MCU) -DF_CPU=$(F_CPU) \
		 -ffunction-sections -fdata-sections -g
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections -Os -Wall

TARGET_HEX = $(OBJ_DIR)/$(TARGET).hex

all: obj obj/main.hex

$(OBJ_DIR)/%.o: %.c
	avr-gcc -c $(CFLAGS) $< -Wall -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

obj/main.elf: $(OBJ)
	avr-gcc $(LDFLAGS) -o $@ $(OBJ) -lc -lm

obj/main.hex: obj/main.elf
	avr-objcopy -O ihex -R eeprom $< $@

obj/main.lss: obj/main.elf
	avr-objdump -h -S $< > $@

reset:
		for STTYF in 'stty -F' 'stty --file' 'stty -f' 'stty <' ; \
		  do $$STTYF /dev/tty >/dev/null 2>&1 && break ; \
		done ; \
		stty -F $(ARD_PORT) hupcl ; \
		(sleep 0.1 2>/dev/null || sleep 1) ; \
		stty -F $(ARD_PORT) -hupcl

upload: obj/main.hex reset
	avrdude -q -V -p $(MCU) \
			-C /etc/avrdude.conf \
			-c arduino \
			-b 115200 \
			-P $(ARD_PORT) \
			-U flash:w:obj/main.hex:i

clean:
	rm -rf obj/

size: obj/main.elf
	avr-size obj/main.elf

.PHONY: upload reset clean size
