TARGET=tag-proximity$(EXT)
ARCH=nrf5
FORMAT=bin
CPU=$(ARCH)1822
VARIANT=xxAB
DEBUG=-g
OPTIM=-Os -mword-relocations
CUSTOM_KEY=inc/custom-encryption-key.h

# determine if we have custom encryption keys
ifeq ("$(wildcard $(CUSTOM_KEY))","")
	ENCRYPTION_KEY:=
else
	ENCRYPTION_KEY:=-DCUSTOM_ENCRYPTION_KEY
endif

APP_CFLAGS=-Iinc -Ilib/heatshrink -std=gnu99 -fgnu89-inline -D __USE_CMSIS $(ENCRYPTION_KEY) $(FEATURE)
APP_LDFLAGS=-lm

APP_SRC= \
  lib/heatshrink/heatshrink_encoder.c \
  src/log.c \
  src/rng.c \
  src/acc.c \
  src/adc.c \
  src/aes.c \
  src/flash.c \
  src/radio.c \
  src/timer.c \
  src/main.c

APP_SRC+=$(IMAGES_C)

all: prox_tag

prox_tag: $(TARGET).$(FORMAT)

marker_tag: marker_tag_far

marker_tag_far: EXT=-marker-far
marker_tag_far: FEATURE=-D MARKER_TAG_FAR
marker_tag_far: prox_tag

marker_tag_near: EXT=-marker-near
marker_tag_near: FEATURE=-D MARKER_TAG_NEAR
marker_tag_near: prox_tag

log_tag: EXT=-log-tag
log_tag: FEATURE=-D LOG_TAG
log_tag: prox_tag

app_clean:
	find src lib -name '*.o' -exec rm \{\} \;

key:
	printf "static const TAES g_default_key = {\n\t`od -v -t x1 -N 16 /dev/urandom | grep 0000000 | sed 's/^[0 ]* /0x/g' | sed 's/  */, 0x/g'`\n};\n" > $(CUSTOM_KEY)

clean_all: clean
	rm -f README $(CUSTOM_KEY)

indent:
	find src inc -iname '*.[ch]' -exec indent -c81 -i4 -cli4 -bli0 -ts 4 \{\} \;
	rm -f src/*.[ch]~ inc/*.[ch]~

include ../core/Makefile.rules
