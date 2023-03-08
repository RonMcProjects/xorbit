PROGNAME := xorbit
CFLAGS += -Wall -Wextra -Werror
LDFLAGS += -lm -lX11

all:
	${CC} ${PROGNAME}.c ${CFLAGS} ${LDFLAGS} -o ${PROGNAME}

clean:
	${RM} ${PROGNAME}.o ${PROGNAME} ${PROGNAME}.exe
