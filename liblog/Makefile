CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../ -I../safe-iop/include
OBJS := logd_write.o
OBJS +=	event_tag_map.o fake_log_device.o

all : liblog.a

liblog.a : $(OBJS)
	$(AR) $@ $(OBJS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.a


