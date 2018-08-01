CC := gcc
LD := ld
RM := rm -f
AR := ar rcs

CFLAGS := -I../ -I. -I../libnativehelper/include/nativehelper
CFLAGS += -fstrict-aliasing -Wstrict-aliasing=2 -fno-align-jumps
CFLAGS += -DWITH_PROFILER -DWITH_DEBUGGER -DDVM_RESOLVER_CACHE=0
CFLAGS += -DDVM_SHOW_EXCEPTION=1 -DOS_SHARED_LIB_FORMAT_STR="\"lib%s.so\""
CFLAGS += -DDVM_NO_ASM_INTERP -g3 -O0

#__CYGWIN__
CFLAGS += -DHAVE_SYS_UIO_H
# need libffi, see https://sourceware.org/libffi/


#???
# CFLAGS += -DLOG_NDEBUG

# LIBS := ../libdex/libdex.a ../liblog/liblog.a -lz -lffi

OBJS := AllocTracker.o AtomicCache.o CheckJni.o Ddm.o Debugger.o DvmDex.o 
OBJS += Exception.o Hash.o IndirectRefTable.o Init.o InlineNative.o Inlines.o 
OBJS += Intern.o Jni.o JarFile.o LinearAlloc.o Misc.o Native.o PointerSet.o 
OBJS += Profile.o Properties.o RawDexFile.o ReferenceTable.o SignalCatcher.o 
OBJS += StdioConverter.o Sync.o TestCompability.o Thread.o UtfString.o 

OBJS += alloc/clz.o alloc/Alloc.o alloc/HeapBitmap.o
OBJS += alloc/HeapDebug.o alloc/HeapSource.o alloc/HeapTable.o
OBJS += alloc/HeapWorker.o alloc/Heap.o alloc/MarkSweep.o
OBJS += alloc/DdmHeap.o

OBJS += analysis/CodeVerify.o analysis/DexOptimize.o analysis/DexVerify.o 
OBJS += analysis/ReduceConstants.o analysis/RegisterMap.o analysis/VerifySubs.o 

OBJS += interp/Interp.o interp/Stack.o

OBJS += jdwp/ExpandBuf.o jdwp/JdwpAdb.o jdwp/JdwpConstants.o jdwp/JdwpEvent.o
OBJS += jdwp/JdwpHandler.o jdwp/JdwpMain.o jdwp/JdwpSocket.o 
	
OBJS += mterp/Mterp.o mterp/out/InterpC-portstd.o 
OBJS += mterp/out/InterpC-portdbg.o

OBJS += native/InternalNative.o native/dalvik_system_DexFile.o

OBJS += native/dalvik_system_SamplingProfiler.o
OBJS += native/dalvik_system_VMDebug.o
OBJS += native/dalvik_system_VMRuntime.o
OBJS += native/dalvik_system_VMStack.o
OBJS += native/dalvik_system_Zygote.o

OBJS += native/java_lang_Class.o
OBJS += native/java_lang_Object.o
OBJS += native/java_lang_Runtime.o
OBJS += native/java_lang_String.o
OBJS += native/java_lang_System.o
OBJS += native/java_lang_SystemProperties.o
OBJS += native/java_lang_Throwable.o
OBJS += native/java_lang_VMClassLoader.o
OBJS += native/java_lang_VMThread.o
OBJS += native/java_lang_reflect_AccessibleObject.o
OBJS += native/java_lang_reflect_Array.o
OBJS += native/java_lang_reflect_Constructor.o
OBJS += native/java_lang_reflect_Field.o
OBJS += native/java_lang_reflect_Method.o
OBJS += native/java_lang_reflect_Proxy.o
OBJS += native/java_security_AccessController.o
OBJS += native/java_util_concurrent_atomic_AtomicLong.o

OBJS += native/org_apache_harmony_dalvik_NativeTestTarget.o
OBJS += native/org_apache_harmony_dalvik_ddmc_DdmServer.o
OBJS += native/org_apache_harmony_dalvik_ddmc_DdmVmInternal.o
OBJS += native/sun_misc_Unsafe.o
OBJS += native/SystemThread.o

OBJS += oo/AccessCheck.o oo/Array.o oo/Class.o
OBJS += oo/Object.o oo/Resolve.o oo/TypeCheck.o

OBJS += reflect/Annotation.o reflect/Proxy.o reflect/Reflect.o
OBJS += test/AtomicSpeed.o test/TestHash.o test/TestIndirectRefTable.o

OBJS += arch/generic/Call.o arch/generic/Hints.o

# OBJS += mterp/cstubs/entry.o mterp/cstubs/enddefs.o mterp/cstubs/stubdefs.o
OBJS += mterp/out/InterpC-allstubs.o

all : libdvm.a

vm.exe : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

libdvm.a : $(OBJS)
	$(AR) $@ $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.S
	$(CC) $(CFLAGS) -o $@ -c $<

clean :
	$(RM) $(OBJS) *.a

