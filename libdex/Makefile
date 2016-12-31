CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../ -I../safe-iop/include
OBJS := CmdUtils.o DexCatch.o DexClass.o DexDataMap.o DexFile.o
OBJS += DexInlines.o DexProto.o DexSwapVerify.o InstrUtils.o
OBJS += Leb128.o OptInvocation.o sha1.o SysUtil.o ZipArchive.o

all : libdex.a

libdex.a : $(OBJS)
	$(AR) $@ $(OBJS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.a


