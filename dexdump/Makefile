CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../ -I../safe-iop/include 

#
# assertion "i == targetLen + arrayDepth * 2" failed: file "DexDump.c", line 169, function: descriptorToDot
#
CFLAGS += -DNDEBUG

LIBS := ../libdex/libdex.a ../liblog/liblog.a -lz
OBJS := DexDump.o OpCodeNames.o

all : dexdump.exe

dexdump.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.exe *.exe.stackdump

test : all
	./dexdump -d ../vendor/app.dex > ../vendor/app.dex.txt
