#need libffi

all:

clean:
	make -C ./liblog clean
	make -C ./libdex clean
	make -C ./vm clean
	make -C ./libnativehelper clean
	make -C ./dexdump clean
	make -C ./dexlist clean
	make -C ./dexopt clean
	make -C ./dvz clean
	make -C ./dalvikvm clean
