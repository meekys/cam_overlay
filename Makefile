DISPLAY:=bmc

OBJS=cam_overlay.o common.o display.o $(DISPLAY)/display.o png_texture.o
BIN=cam_overlay.bin
CFLAGS+=-I$(DISPLAY)
LDFLAGS+=-lpng

include ./Makefile.include
include ./$(DISPLAY)/Makefile.include