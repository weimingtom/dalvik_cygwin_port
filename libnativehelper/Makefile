CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I.. -I./include/nativehelper
OBJS := Register.o JNIHelp.o

all : libnativehelper.a

libnativehelper.a : $(OBJS)
	$(AR) $@ $(OBJS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.a


