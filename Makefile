PROJECT		= wibuttonc
F_CPU 		= 2000000
PART		= atxmega16d4
CFLAGS		= -c -Wall -mmcu=$(PART) -DF_CPU=$(F_CPU) -Os
OBJECTS		= nrf24.o $(PROJECT).o radioPinFunctions.o
CC		= avr-g++
OBJCOPY 	= avr-objcopy
DUDEPART = x16d4
TOOL = dragon_pdi
PORT = usb

all: main.hex

main.hex: main.elf
	$(OBJCOPY) -Oihex $< $@

%.o: %.c %.h
	$(CC) -c $(CFLAGS) -o $@ $<

install: main.elf
	avrdude -B 1 -v -c $(TOOL) -P $(PORT) -p $(DUDEPART) -U flash:w:main.hex

binary: $(PROJECT).bin

main.elf: $(OBJECTS)
	$(CC) -mmcu=$(PART) -Wl,-Map,main.map -o main.elf $(OBJECTS)

$(PROJECT).bin: main.elf
	$(OBJCOPY) -Obinary main.elf $(PROJECT).bin

all: $(P) binary

size: main.elf
	avr-size main.elf

clean:
	rm -f $(OBJECTS) main.hex main.elf main.map
