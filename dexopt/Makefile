CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../ -I../safe-iop/include -I../vm
CFLAGS += -I../libnativehelper/include/nativehelper

CFLAGS += -DUSE_DL_PREFIX=1 -DANDROID=1 -DNO_MALLINFO=1 
# for sched_policy.c
CFLAGS += -DHAVE_SCHED_H -DHAVE_PTHREADS=1

LIBS := ../vm/libdvm.a ../libdex/libdex.a ../liblog/liblog.a -lz -lffi
OBJS := OptMain.o mspace.o dlmalloc.o ashmem-host.o atomic.o sched_policy.o 
OBJS += Register.o
OBJS += process_name.o properties.o

all : dexopt.exe

dexopt.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.exe
