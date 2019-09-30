CC=gcc
CFLAGS=-lX11 -lGL -lGLU -lGLX -lm -Iinclude -Wall -g -ldl -lasound -lpthread
VPATH=source
OFILES=main.o pl_posix.o keys.o glfns.o wavefront.o vmath.o gfx.o netpbm.o sound.o map.o game.o

all: $(OFILES)
	$(CC) $(CFLAGS) $(OFILES)

map: mapper.o
	$(CC) mapper.o -lforms -o map

clean:
	rm *.o a.out map

