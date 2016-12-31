file dalvikvm.exe
set args -help

# b dvmLockThreadList
# b dvmPropertiesStartup
# b dvmProcessOptions
# b dvmCheckAsmConstants


# thread init
#     if (!dvmThreadStartup())
#        goto fail;
b dvmThreadStartup


b dvmLockThreadList
# b dvmInitMutex

# b pthread_mutex_lock
b __android_log_print
b dvmUsage

r


# Xbootclasspath
# ./dalvikvm.exe -Xbootclasspath:.. Hello
# ./dalvikvm.exe -Xbootclasspath:foo.jar -classpath foo.jar Foo

