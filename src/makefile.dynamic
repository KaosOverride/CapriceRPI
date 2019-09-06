

CC      = g++
ASM	= as

#STATICDIR=./SDL
#STATICLIBS = $(STATICDIR)/lib/libSDL_image.a $(STATICDIR)/lib/libSDL_ttf.a $(STATICDIR)/lib/libSDLmain.a  $(STATICDIR)/lib/libSDL.a   -lasound -lpthread -lpulse-simple -lpulse -lX11 -lXext -lXrandr -lbcm_host -ldirectfb -lcaca -lts
#STATICBIN=./SDL/bin/

GFLAGS = -Wall `$(STATICBIN)sdl-config --cflags` -L/opt/vc/lib -I/usr/local/include -L/usr/local/lib -DLINUX 
ASMFLAGS =


#ifdef RELEASE
	CFLAGS  = $(GFLAGS) -O2 -funroll-loops -ffast-math -fomit-frame-pointer -fno-strength-reduce -finline-functions -s 
#else
	#CFLAGS  = $(GFLAGS) -gstabs+ -static
#endif

ifdef STATICDIR
	LIBS =  $(STATICLIBS) -lz -lm -ldl -lfreetype -lpng -ljpeg 
else
	LIBS =  -lSDL -lSDL_ttf -lSDL_image -lz -lm -ldl -lfreetype -lpng -ljpeg 
endif

capriceRPI: cap32.cpp menu.o autotype.o crtc.o fdc.o psg.o tape.o z80.o cap32.h z80.h autoboot.c vjoystick.c
	$(CC) $(CFLAGS) $(IPATHS) -o capriceRPI cap32.cpp menu.o crtc.o fdc.o psg.o tape.o z80.o autotype.o $(LIBS)

crtc.o: crtc.c videodraw/draw_8bpp.c videodraw/draw_16bpp.c videodraw/draw_24bpp.c videodraw/draw_32bpp.c cap32.h crtc.h z80.h
	$(CC) $(CFLAGS) -c crtc.c

fdc.o: fdc.c cap32.h z80.h
	$(CC) $(CFLAGS) -c fdc.c

psg.o: psg.c cap32.h z80.h
	$(CC) $(CFLAGS) -c psg.c

tape.o: tape.c cap32.h tape.h z80.h
	$(CC) $(CFLAGS) -c tape.c

z80.o: z80.c z80.h cap32.h
	$(CC) $(CFLAGS) -c z80.c

menu.o: menu.c cap32.h tape.h z80.h
	$(CC) $(CFLAGS) -c menu.c

autotype.o: autotype.c autotype.h
	$(CC) $(CFLAGS) -c autotype.c

clean:
	rm *.o capriceRPI
	
