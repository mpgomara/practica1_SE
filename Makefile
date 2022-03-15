#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -O0 -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I./board $(COMMONFLAGS) -I./board/src $(COMMONFLAGS) -I./drivers $(COMMONFLAGS) -I./CMSIS $(COMMONFLAGS) -I./utilities $(COMMONFLAGS) -DCPU_MKL46Z256VLL4
#CFLAGS=-I./board/src $(COMMONFLAGS) -Wl,-V
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,-t,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

TARGETH=hello_world
TARGET=led_blinky

foo=board/pin_mux_h.c hello_world.c board/pin_mux_h.o hello_world.o

fooh=led_blinky.c board/pin_mux.c led_blinky.o board/pin_mux.o

SRC=$(wildcard *.c board/*.c board/src/*.c CMSIS/*.c drivers/*.c utilities/*.c)
OBJ=$(patsubst %.c, %.o, $(filter-out $(foo),$(SRC)))
OBJH=$(patsubst %.c, %.o, $(filter-out $(fooh),$(SRC)))
#OBJH=$(patsubst %.c, %.o, $(SRC))


all: build size
allh: buildh sizeh
build: elf srec bin
buildh: elfh srech binh
elf: $(TARGET).elf
elfh: $(TARGETH).elf
srec: $(TARGET).srec
srech: $(TARGETH).srec
bin: $(TARGET).bin
binh: $(TARGETH).bin

clean:
	$(RM) *.srec *.elf *.bin *.map $(OBJH) $(OBJ)

$(TARGETH).elf: $(OBJH)
	$(LD) $(LDFLAGS) $(OBJH) $(LDLIBS) -o $@

$(TARGET).elf: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

size:
	$(SIZE) $(TARGET).elf

sizeh:
	$(SIZE) $(TARGETH).elf

flash_led: all
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"

flash_hello: allh
	openocd -f openocd.cfg -c "program $(TARGETH).elf verify reset exit"

