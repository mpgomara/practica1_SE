#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -O0 -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I./board $(COMMONFLAGS) -I./board/src $(COMMONFLAGS) -I./drivers $(COMMONFLAGS) -I./CMSIS $(COMMONFLAGS) -I./utilities $(COMMONFLAGS)
#CFLAGS=-I./board/src $(COMMONFLAGS) -Wl,-V
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,-t,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

#TARGET=hello_world
TARGET=led_blinky

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

#include CPU_MKL46Z256VLL4

all: build size
build: elf srec bin
elf: $(TARGET).elf
srec: $(TARGET).srec
bin: $(TARGET).bin

clean:
	$(RM) $(TARGET).srec $(TARGET).elf $(TARGET).bin $(TARGET).map $(OBJ)

$(TARGET).elf: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

size:
	$(SIZE) $(TARGET).elf

flash_led: all
	#TARGET:=led_blinky
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"

flash_hello: all
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"
