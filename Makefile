# ATTiny20
MCU = attiny20

# Sourcefiles
TARGET = main
SRC = src/$(TARGET).c

# Programmer settings
AVRDUDE_PROGRAMMER = avrispmkii
AVRDUDE_PORT = usb

# set flags here
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -C ./avrdude/avrdude.conf
AVR_GCC_FLAGS = -Wall -g -Os -std=c99 

# Comment out if build files shall just be dumped into the working directory
OUTDIR = build/

all:
		mkdir -p $(OUTDIR)
			avr-gcc $(SRC) $(AVR_GCC_FLAGS) -mmcu=$(MCU) -o $(OUTDIR)$(TARGET).elf
				avr-objcopy -j .text -j .data -O ihex $(OUTDIR)$(TARGET).elf $(OUTDIR)$(TARGET).hex
					avrdude $(AVRDUDE_FLAGS) -U flash:w:$(OUTDIR)$(TARGET).hex

clean:
		rm -r $(OUTDIR)
