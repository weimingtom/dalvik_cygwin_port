/*
 * Copyright 2006 The Android Open Source Project
 *
 * JNI helper functions.
 */
#include "jni.h"
#include "AndroidSystemNatives.h"

#include <stdio.h>

/*
 * Register all methods for system classes.
 *
 * Remember to add the declarations to include/nativehelper/JavaSystemNatives.h.
 */
int jniRegisterSystemMethods(JNIEnv* env)
{
    int result = -1;

    (*env)->PushLocalFrame(env, 128);

    result = 0;

bail:
    (*env)->PopLocalFrame(env, NULL);
    return result;
}
