CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../
LIBS := ../libdex/libdex.a ../liblog/liblog.a -lz
OBJS := DexList.o

all : dexlist.exe

dexlist.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.exe

test : all
	./dexlist ../vendor/app.dex
