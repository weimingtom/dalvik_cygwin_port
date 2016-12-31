CC := gcc
LD := ld
RM := rm -f
AR := ar rcs
CFLAGS := -I../
LIBS := 
# ../libdex/libdex.a -lz
OBJS := dvz.o

all : dvz.exe

dvz.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	$(RM) $(OBJS) *.exe
