OBJS=cam_overlay.o common.o display.o png_texture.o matrix.o
BIN=cam_overlay.bin
LDFLAGS+=-lilclient -lpng

include ./Makefile.include
