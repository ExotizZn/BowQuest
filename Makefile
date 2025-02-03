CC       := gcc
CFLAGS   := -O3 `pkg-config sdl3 --cflags --libs` -lSDL3_image -lSDL3_ttf -lm
SRC      := ./src
INCLUDE  := ./include
OBJFILES := main.o camera.o player.o enemy.o projectile.o utils.o menu.o upgrade.o fonts.o SDL3_gfxPrimitives.o SDL3_rotozoom.o
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

menu.o : ${SRC}/main.c
	${CC} -c ${SRC}/menu.c

fonts.o : ${SRC}/main.c
	${CC} -c ${SRC}/fonts.c

upgrade.o : ${SRC}/main.c
	${CC} -c ${SRC}/upgrade.c

SDL3_gfxPrimitives.o : ${SRC}/main.c
	${CC} -c ./libs/SDL3_gfx/SDL3_gfxPrimitives.c

SDL3_rotozoom.o : ${SRC}/main.c
	${CC} -c ./libs/SDL3_gfx/SDL3_rotozoom.c

clean :
	rm -f *~ *.o