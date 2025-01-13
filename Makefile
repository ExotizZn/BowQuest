CC       := gcc
CFLAGS   := -O3 `pkg-config sdl3 --cflags --libs` -lSDL3_image
SRC      := ./src
INCLUDE  := ./include
OBJFILES := main.o camera.o player.o enemy.o projectile.o utils.o
TARGET   := main

all: $(TARGET) clean

${TARGET}: ${OBJFILES}
	${CC} ${OBJFILES} -o ${TARGET} ${CFLAGS}

main.o : ${SRC}/main.c
	${CC} -c ${SRC}/main.c

camera.o : ${SRC}/main.c
	${CC} -c ${SRC}/camera.c

player.o : ${SRC}/main.c
	${CC} -c ${SRC}/player.c

enemy.o : ${SRC}/main.c
	${CC} -c ${SRC}/enemy.c

projectile.o : ${SRC}/main.c
	${CC} -c ${SRC}/projectile.c

utils.o : ${SRC}/main.c
	${CC} -c ${SRC}/utils.c

clean :
	rm -f *~ *.o