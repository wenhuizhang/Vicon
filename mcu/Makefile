###############################################################################
# Makefile for the project mcu
###############################################################################

## General Flags
PROJECT = mcu
MCU = atmega328p
TARGET = mcu.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=mcu.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I include/ -I $(MCU_INTERFACE)/include 

## Objects that must be built in order to link
OBJECTS = obj/mcu.o obj/pwm.o obj/pulsecapture.o obj/modeindicator.o obj/usart.o obj/mode.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) obj/mcu.hex obj/mcu.eep obj/mcu.lss

## Compile
obj/mcu.o: src/mcu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

obj/pwm.o: src/pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

obj/pulsecapture.o: src/pulsecapture.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

obj/modeindicator.o: src/modeindicator.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

obj/usart.o: src/usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

obj/mode.o: src/mode.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@


##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

obj/%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

obj/%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

obj/%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

#size: ${TARGET}
	#@echo
	#@avr-size -Bt --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) mcu.elf dep/* obj/mcu.hex obj/mcu.eep obj/mcu.lss obj/mcu.map mcu.map obj/*


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

