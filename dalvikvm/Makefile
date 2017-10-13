CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../libnativehelper/include/nativehelper -I.. -DUSE_DL_PREFIX=1
CFLAGS += -DANDROID=1 -DNO_MALLINFO=1 
# for sched_policy.c
CFLAGS += -DHAVE_SCHED_H -DHAVE_PTHREADS=1

# -DUSE_MAX_ALLOWED_FOOTPRINT=1 
LIBS := ../vm/libdvm.a ../libdex/libdex.a ../liblog/liblog.a
LIBS += -lz -lffi

OBJS := Main.o atomic.o dlmalloc.o mspace.o ashmem-host.o sched_policy.o

# modified, removing register lib
OBJS += Register.o
# LIBS += ../libnativehelper/libnativehelper.a

# /system/core/libcutils/mspace.c
# -DMSPACES=1 -DONLY_MSPACES=1

all : dalvikvm.exe

dalvikvm.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.exe *.exe.stackdump

test :
	./dalvikvm -cp foo.jar Foo
