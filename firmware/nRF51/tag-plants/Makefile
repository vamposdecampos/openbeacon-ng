TARGET=tag-plants
BOARD?=wt51822_s4at
ARCH=nrf5
CPU=$(ARCH)1822
VARIANT=xxAB
DEBUG=-g
OPTIM=-Os -mword-relocations

APP_CFLAGS=-Ilib/inc -std=gnu99 -fgnu89-inline -D__USE_CMSIS
APP_LDFLAGS=-lm

APP_SRC=entry.c

LIB_SRC= \
  lib/src/acc.c \
  lib/src/flash.c \
  lib/src/radio.c \
  lib/src/main.c

APP_SRC+=$(LIB_SRC) $(IMAGES_C)

OPTION_SRC= \
  adc.c \
  timer.c

all: $(TARGET).bin

publish: clean $(TARGET).bin
	scp $(TARGET).bin openbeacon.org:/home/wwwrun/open.bitmanufaktur.com/web/www/people/milosch/nrf5/

app_clean:
	find . -name '*.o' -exec rm \{\} \;

clean_all: clean
	rm -f README $(CUSTOM_KEY)

include ../core/Makefile.rules
