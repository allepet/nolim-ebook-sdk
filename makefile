CC=arm-linux-gnueabi-gcc
INCLUDEPATH=-I'libs/directfb/include' -I'libs/directfb/include/directfb' -I'libs/directfb/lib/' -I'/home/ubuntupruebas/Escritorio/curl-7.51.0/builds/include'
LIBS=-Llibs/ -ldirectfb -ldirect -lfusion -lz -lm

all:
	$(CC) main.c $(INCLUDEPATH) $(LIBS) -o app

