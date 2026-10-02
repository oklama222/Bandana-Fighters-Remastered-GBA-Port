# Étape 5 : prototype minimal -> BandanaFightersGBA.gba
# Prérequis : devkitPro + devkitARM (arm-none-eabi-gcc) et gbafix.
#   Windows : installeur devkitPro | Linux/macOS : dkp-pacman -S gba-dev
DEVKITARM ?= /opt/devkitpro/devkitARM
PATH := $(DEVKITARM)/bin:/opt/devkitpro/tools/bin:$(PATH)
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
TARGET  := BandanaFightersGBA
ARCH    := -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) -O2 -Wall -Wextra -ffunction-sections -fdata-sections
LDFLAGS := $(ARCH) -specs=gba.specs -Wl,--gc-sections
SRC     := bandana.c

all: build/$(TARGET).gba
build/$(TARGET).elf: $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $@
build/$(TARGET).gba: build/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	gbafix $@ -tBANDANAFGT -cBFGA -mXX
	@ls -l $@
run: all
	mgba build/$(TARGET).gba
clean:
	rm -rf build
.PHONY: all clean
