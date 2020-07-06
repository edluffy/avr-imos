PKG=main
BIN=${PKG}
OBJS=${PKG}.o ui.o io.o
MCU=atmega644p

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS= -mmcu=${MCU} -isystem/usr/local/opt/avr-gcc/avr/include -DF_CPU=12000000 -Wall -Os -u vfprintf -lprintf_flt -lm -L $(shell pwd)/lcdlib
PORT=

# $@ is the name of the current target
# $< is the name of the first prerequisite
# $^ is the list of all the prerequisites.

# llcd must be at end!

${BIN}.hex: ${BIN}.elf
	${OBJCOPY} -O ihex $< $@

${BIN}.elf: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^ -llcd

upload: ${BIN}.hex
	avrdude -v -c usbasp -p ${MCU} -U flash:w:$<

clean:
	rm -f ${BIN}.elf ${BIN}.hex ${OBJS}
