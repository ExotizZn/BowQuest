CC       := gcc
CFLAGS   := -O3 `pkg-config sdl3 --cflags --libs` -lSDL3_mixer -lSDL3_image -lSDL3_ttf -lm
SRC      := ./src
INCLUDE  := ./include
OBJFILES := main.o camera.o player.o enemy.o projectile.o utils.o menu.o fonts.o mouse.o save.o SDL3_gfxPrimitives.o SDL3_rotozoom.o upgrade.o
TARGET   := main

all: $(TARGET) clean

${TARGET}: ${OBJFILES}
	${CC} ${OBJFILES} -o ${TARGET} ${CFLAGS}

main.o : ${SRC}/main.c
	${CC} -c ${SRC}/main.c

camera.o : ${SRC}/camera.c
	${CC} -c ${SRC}/camera.c

player.o : ${SRC}/player.c
	${CC} -c ${SRC}/player.c

enemy.o : ${SRC}/enemy.c
	${CC} -c ${SRC}/enemy.c

projectile.o : ${SRC}/projectile.c
	${CC} -c ${SRC}/projectile.c

utils.o : ${SRC}/utils.c
	${CC} -c ${SRC}/utils.c

menu.o : ${SRC}/menu.c
	${CC} -c ${SRC}/menu.c

fonts.o : ${SRC}/fonts.c
	${CC} -c ${SRC}/fonts.c

mouse.o : ${SRC}/mouse.c
	${CC} -c ${SRC}/mouse.c

save.o : ${SRC}/save.c
	${CC} -c ${SRC}/save.c

upgrade.o : ${SRC}/upgrade.c
	${CC} -c ${SRC}/upgrade.c

SDL3_gfxPrimitives.o : ./libs/SDL3_gfx/SDL3_gfxPrimitives.c
	${CC} -c ./libs/SDL3_gfx/SDL3_gfxPrimitives.c

SDL3_rotozoom.o : ./libs/SDL3_gfx/SDL3_rotozoom.c
	${CC} -c ./libs/SDL3_gfx/SDL3_rotozoom.c

clean:
	rm -f *~ *.o