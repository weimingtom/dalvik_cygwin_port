/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "OSMemory"
#include "JNIHelp.h"
#include "AndroidSystemNatives.h"
#include "utils/misc.h"
#include "utils/Log.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Cached dalvik.system.VMRuntime pieces.
 */
static struct {
    jmethodID method_trackExternalAllocation;
    jmethodID method_trackExternalFree;

    jobject runtimeInstance;
} gIDCache;

static const int MMAP_READ_ONLY = 1;
static const int MMAP_READ_WRITE = 2;
static const int MMAP_WRITE_COPY = 4;

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    littleEndian
 * Signature: ()Z
 */
static jboolean harmony_nio_littleEndian(JNIEnv *_env, jclass _this) {
    long l = 0x01020304;
    unsigned char* c = (unsigned char*)&l;
    return (*c == 0x04) ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getPointerSizeImpl
 * Signature: ()I
 */
static jint harmony_nio_getPointerSizeImpl(JNIEnv *_env, jclass _this) {
    return sizeof(void *);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    mallocImpl
 * Signature: (I)I
 */
static jint harmony_nio_mallocImpl(JNIEnv *_env, jobject _this, jint size) {
    jboolean allowed = _env->CallBooleanMethod(gIDCache.runtimeInstance,
        gIDCache.method_trackExternalAllocation, (jlong) size);
    if (!allowed) {
        LOGW("External allocation of %d bytes was rejected\n", size);
        jniThrowException(_env, "java/lang/OutOfMemoryError", NULL);
        return 0;
    }

    LOGV("OSMemory alloc %d\n", size);
    void *returnValue = malloc(size + sizeof(jlong));
    if (returnValue == NULL) {
        jniThrowException(_env, "java/lang/OutOfMemoryError", NULL);
        return 0;
    }

    /*
     * Tuck a copy of the size at the head of the buffer.  We need this
     * so harmony_nio_freeImpl() knows how much memory is being freed.
     */
    jlong* adjptr = (jlong*) returnValue;
    *adjptr++ = size;
    return (jint)adjptr;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    freeImpl
 * Signature: (I)V
 */
static void harmony_nio_freeImpl(JNIEnv *_env, jobject _this, jint pointer) {
    jlong* adjptr = (jlong*) pointer;
    jint size = *--adjptr;
    LOGV("OSMemory free %d\n", size);
    _env->CallVoidMethod(gIDCache.runtimeInstance,
        gIDCache.method_trackExternalFree, (jlong) size);
    free((void *)adjptr);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    memset
 * Signature: (IBJ)V
 */
static void harmony_nio_memset(JNIEnv *_env, jobject _this, jint address, 
        jbyte value, jlong length) {
    memset ((void *) ((jint) address), (jbyte) value, (jlong) length);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    memmove
 * Signature: (IIJ)V
 */
static void harmony_nio_memmove(JNIEnv *_env, jobject _this, jint destAddress, 
        jint srcAddress, jlong length) {
    memmove ((void *) ((jint) destAddress), (const void *) ((jint) srcAddress), 
        (jlong) length);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getByteImpl
 * Signature: (I)B
 */
static jbyte harmony_nio_getByteImpl(JNIEnv *_env, jobject _this, 
        jint pointer) {
    jbyte returnValue = *((jbyte *)pointer);
    return returnValue;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getBytesImpl
 * Signature: (I[BII)V
 */
static void harmony_nio_getBytesImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jbyteArray dst, jint offset, jint length) {
    jbyte* src = reinterpret_cast<jbyte*>(static_cast<uintptr_t>(pointer));
    _env->SetByteArrayRegion(dst, offset, length, src);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    putByteImpl
 * Signature: (IB)V
 */
static void harmony_nio_putByteImpl(JNIEnv *_env, jobject _this, jint pointer,
        jbyte val) {
    *((jbyte *)pointer) = val;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    putBytesImpl
 * Signature: (I[BII)V
 */
static void harmony_nio_putBytesImpl(JNIEnv *_env, jobject _this,
        jint pointer, jbyteArray src, jint offset, jint length) {
    jbyte* dst = reinterpret_cast<jbyte*>(static_cast<uintptr_t>(pointer));
    _env->GetByteArrayRegion(src, offset, length, dst);
}

static void
swapShorts(jshort *shorts, int count) {
    jbyte *src = (jbyte *) shorts;
    jbyte *dst = src;
    for (int i = 0; i < count; ++i) {
        jbyte b0 = *src++;
        jbyte b1 = *src++;
        *dst++ = b1;
        *dst++ = b0;
    }
}

static void
swapInts(jint *ints, int count) {
    jbyte *src = (jbyte *) ints;
    jbyte *dst = src;
    for (int i = 0; i < count; ++i) {
        jbyte b0 = *src++;
        jbyte b1 = *src++;
        jbyte b2 = *src++;
        jbyte b3 = *src++;
        *dst++ = b3;
        *dst++ = b2;
        *dst++ = b1;
        *dst++ = b0;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    setShortArrayImpl
 * Signature: (I[SIIZ)V
 */
static void harmony_nio_setShortArrayImpl(JNIEnv *_env, jobject _this,
       jint pointer, jshortArray src, jint offset, jint length, jboolean swap) {
    jshort* dst = reinterpret_cast<jshort*>(static_cast<uintptr_t>(pointer));
    _env->GetShortArrayRegion(src, offset, length, dst);
    if (swap) {
        swapShorts(dst, length);
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    setIntArrayImpl
 * Signature: (I[IIIZ)V
 */
static void harmony_nio_setIntArrayImpl(JNIEnv *_env, jobject _this,
       jint pointer, jintArray src, jint offset, jint length, jboolean swap) {
    jint* dst = reinterpret_cast<jint*>(static_cast<uintptr_t>(pointer));
    _env->GetIntArrayRegion(src, offset, length, dst);
    if (swap) {
        swapInts(dst, length);
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getShortImpl
 * Signature: (I)S
 */
static jshort harmony_nio_getShortImpl(JNIEnv *_env, jobject _this, 
        jint pointer) {
    if ((pointer & 0x1) == 0) {
        jshort returnValue = *((jshort *)pointer);
        return returnValue;
    } else {
        // Handle unaligned memory access one byte at a time
        jshort s;
        unsigned char *src = (unsigned char *) pointer;
        unsigned char *dst = (unsigned char *) &s;
        dst[0] = src[0];
        dst[1] = src[1];
        return s;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    petShortImpl
 * Signature: (IS)V
 */
static void harmony_nio_putShortImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jshort value) {
    if ((pointer & 0x1) == 0) {
        *((jshort *)pointer) = value;
    } else {
        // Handle unaligned memory access one byte at a time
        unsigned char *src = (unsigned char *) &value;
        unsigned char *dst = (unsigned char *) pointer;
        dst[0] = src[0];
        dst[1] = src[1];
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getIntImpl
 * Signature: (I)I
 */
static jint harmony_nio_getIntImpl(JNIEnv *_env, jobject _this, jint pointer) {
    if ((pointer & 0x3) == 0) {
        jint returnValue = *((jint *)pointer);
        return returnValue;
    } else {
        // Handle unaligned memory access one byte at a time
        jint i;
        unsigned char *src = (unsigned char *) pointer;
        unsigned char *dst = (unsigned char *) &i;
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        return i;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    putIntImpl
 * Signature: (II)V
 */
static void harmony_nio_putIntImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jint value) {
    if ((pointer & 0x3) == 0) {
        *((jint *)pointer) = value;
    } else {
        // Handle unaligned memory access one byte at a time
        unsigned char *src = (unsigned char *) &value;
        unsigned char *dst = (unsigned char *) pointer;
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getLongImpl
 * Signature: (I)Ljava/lang/Long;
 */
static jlong harmony_nio_getLongImpl(JNIEnv *_env, jobject _this, 
        jint pointer) {
    if ((pointer & 0x7) == 0) {
        jlong returnValue = *((jlong *)pointer);
        return returnValue;
    } else {
        // Handle unaligned memory access one byte at a time
        jlong l;
        memcpy((void *) &l, (void *) pointer, sizeof(jlong));
        return l;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    putLongImpl
 * Signature: (IJ)V
 */
static void harmony_nio_putLongImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jlong value) {
    if ((pointer & 0x7) == 0) {
        *((jlong *)pointer) = value;
    } else {
        // Handle unaligned memory access one byte at a time
        memcpy((void *) pointer, (void *) &value, sizeof(jlong));
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getFloatImpl
 * Signature: (I)F
 */
static jfloat harmony_nio_getFloatImpl(JNIEnv *_env, jobject _this, 
        jint pointer) {
    if ((pointer & 0x3) == 0) {
        jfloat returnValue = *((jfloat *)pointer);
        return returnValue;
    } else {
        // Handle unaligned memory access one byte at a time
        jfloat f;
        memcpy((void *) &f, (void *) pointer, sizeof(jfloat));
        return f;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    setFloatImpl
 * Signature: (IF)V
 */
static void harmony_nio_putFloatImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jfloat value) {
    if ((pointer & 0x3) == 0) {
        *((jfloat *)pointer) = value;
    } else {
        // Handle unaligned memory access one byte at a time
        memcpy((void *) pointer, (void *) &value, sizeof(jfloat));
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getDoubleImpl
 * Signature: (I)D
 */
static jdouble harmony_nio_getDoubleImpl(JNIEnv *_env, jobject _this, 
        jint pointer) {
    if ((pointer & 0x7) == 0) {
        jdouble returnValue = *((jdouble *)pointer);
        return returnValue;
    } else {
        // Handle unaligned memory access one byte at a time
        jdouble d;
        memcpy((void *) &d, (void *) pointer, sizeof(jdouble));
        return d;
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    putDoubleImpl
 * Signature: (ID)V
 */
static void harmony_nio_putDoubleImpl(JNIEnv *_env, jobject _this, jint pointer, 
        jdouble value) {
    if ((pointer & 0x7) == 0) {
        *((jdouble *)pointer) = value;
    } else {
        // Handle unaligned memory access one byte at a time
        memcpy((void *) pointer, (void *) &value, sizeof(jdouble));
    }
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    getAddress
 * Signature: (I)I
 */
static jint harmony_nio_getAddress(JNIEnv *_env, jobject _this, jint pointer) {
    return (jint) * (int *) pointer;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    setAddress
 * Signature: (II)V
 */
static void harmony_nio_setAddress(JNIEnv *_env, jobject _this, jint pointer, 
        jint value) {
    *(int *) pointer = (int) value;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    mmapImpl
 * Signature: (IJJI)I
 */
static jint harmony_nio_mmapImpl(JNIEnv* env, jobject, jint fd, 
        jlong offset, jlong size, jint mapMode) {
    int prot, flags;
    switch (mapMode) {
    case MMAP_READ_ONLY:
        prot = PROT_READ;
        flags = MAP_SHARED;
        break;
    case MMAP_READ_WRITE:
        prot = PROT_READ|PROT_WRITE;
        flags = MAP_SHARED;
        break;
    case MMAP_WRITE_COPY:
        prot = PROT_READ|PROT_WRITE;
        flags = MAP_PRIVATE;
        break;
    default:
        jniThrowIOException(env, EINVAL);
        LOGE("bad mapMode %i", mapMode);
        return -1;
    }

    void* mapAddress = mmap(0, size, prot, flags, fd, offset);
    if (mapAddress == MAP_FAILED) {
        jniThrowIOException(env, errno);
    }
    return reinterpret_cast<uintptr_t>(mapAddress);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    unmapImpl
 * Signature: (IJ)V
 */
static void harmony_nio_unmapImpl(JNIEnv *_env, jobject _this, jint address, 
        jlong size) {
    munmap((void *)address, (size_t)size);
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    loadImpl
 * Signature: (IJ)I
 */
static jint harmony_nio_loadImpl(JNIEnv *_env, jobject _this, jint address, 
        jlong size) {

    if(mlock((void *)address, (size_t)size)!=-1) {
        if(munlock((void *)address, (size_t)size)!=-1) {
              return 0;  /* normally */
        }
    }
    else {
         /* according to linux sys call, only root can mlock memory. */
         if(errno == EPERM) {
             return 0;
         }
    }
    
    return -1;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    isLoadedImpl
 * Signature: (IJ)Z
 */
static jboolean harmony_nio_isLoadedImpl(JNIEnv *_env, jobject _this, 
        jint address, jlong size) {

    static int page_size = getpagesize();
    jboolean result = 0;
    jint m_addr = (jint)address;
    
    int align_offset = m_addr%page_size;// addr should align with the boundary of a page.
    m_addr -= align_offset;
    size   += align_offset;
    int page_count = (size+page_size-1)/page_size;
    
    unsigned char* vec = (unsigned char *) malloc(page_count*sizeof(char));
    
    if (mincore((void *)m_addr, size, (MINCORE_POINTER_TYPE) vec)==0) {
        // or else there is error about the mincore and return false;
        int i;
        for(i=0 ;i<page_count;i++) {
            if(vec[i]!=1) {
                break;
            }
        }
        if(i==page_count) {
            result = 1;
        }
    }
    
    free(vec);
    
    return result;
}

/*
 * Class:     org_apache_harmony_luni_platform_OSMemory
 * Method:    flushImpl
 * Signature: (IJ)I
 */
static jint harmony_nio_flushImpl(JNIEnv *_env, jobject _this, jint address, 
        jlong size) {
    return msync((void *)address, size, MS_SYNC);
}

/*
 * JNI registration
 */
static JNINativeMethod gMethods[] = {
    /* name, signature, funcPtr */
    { "isLittleEndianImpl", "()Z",     (void*) harmony_nio_littleEndian },
    { "getPointerSizeImpl", "()I",     (void*) harmony_nio_getPointerSizeImpl },
    { "malloc",             "(I)I",    (void*) harmony_nio_mallocImpl },
    { "free",               "(I)V",    (void*) harmony_nio_freeImpl },
    { "memset",             "(IBJ)V",  (void*) harmony_nio_memset },
    { "memmove",            "(IIJ)V",  (void*) harmony_nio_memmove },
    { "getByteArray",       "(I[BII)V",(void*) harmony_nio_getBytesImpl },
    { "setByteArray",       "(I[BII)V",(void*) harmony_nio_putBytesImpl },
    { "setShortArray",     "(I[SIIZ)V",(void*) harmony_nio_setShortArrayImpl },
    { "setIntArray",       "(I[IIIZ)V",(void*) harmony_nio_setIntArrayImpl },
    { "getByte",            "(I)B",    (void*) harmony_nio_getByteImpl },
    { "setByte",            "(IB)V",   (void*) harmony_nio_putByteImpl },
    { "getShort",           "(I)S",    (void*) harmony_nio_getShortImpl },
    { "setShort",           "(IS)V",   (void*) harmony_nio_putShortImpl },
    { "getInt",             "(I)I",    (void*) harmony_nio_getIntImpl },
    { "setInt",             "(II)V",   (void*) harmony_nio_putIntImpl },
    { "getLong",            "(I)J",    (void*) harmony_nio_getLongImpl },
    { "setLong",            "(IJ)V",   (void*) harmony_nio_putLongImpl },
    { "getFloat",           "(I)F",    (void*) harmony_nio_getFloatImpl },
    { "setFloat",           "(IF)V",   (void*) harmony_nio_putFloatImpl },
    { "getDouble",          "(I)D",    (void*) harmony_nio_getDoubleImpl },
    { "setDouble",          "(ID)V",   (void*) harmony_nio_putDoubleImpl },
    { "getAddress",         "(I)I",    (void*) harmony_nio_getAddress },
    { "setAddress",         "(II)V",   (void*) harmony_nio_setAddress },
    { "mmapImpl",           "(IJJI)I", (void*) harmony_nio_mmapImpl },
    { "unmapImpl",          "(IJ)V",   (void*) harmony_nio_unmapImpl },
    { "loadImpl",           "(IJ)I",   (void*) harmony_nio_loadImpl },
    { "isLoadedImpl",       "(IJ)Z",   (void*) harmony_nio_isLoadedImpl },
    { "flushImpl",          "(IJ)I",   (void*) harmony_nio_flushImpl }
};
int register_org_apache_harmony_luni_platform_OSMemory(JNIEnv *_env) {
    /*
     * We need to call VMRuntime.trackExternal{Allocation,Free}.  Cache
     * method IDs and a reference to the singleton.
     */
    static const char* kVMRuntimeName = "dalvik/system/VMRuntime";
    jmethodID method_getRuntime;
    jclass clazz;

    clazz = _env->FindClass(kVMRuntimeName);
    if (clazz == NULL) {
        LOGE("Unable to find class %s\n", kVMRuntimeName);
        return -1;
    }
    gIDCache.method_trackExternalAllocation = _env->GetMethodID(clazz,
        "trackExternalAllocation", "(J)Z");
    gIDCache.method_trackExternalFree = _env->GetMethodID(clazz,
        "trackExternalFree", "(J)V");
    method_getRuntime = _env->GetStaticMethodID(clazz,
        "getRuntime", "()Ldalvik/system/VMRuntime;");

    if (gIDCache.method_trackExternalAllocation == NULL ||
        gIDCache.method_trackExternalFree == NULL ||
        method_getRuntime == NULL)
    {
        LOGE("Unable to find VMRuntime methods\n");
        return -1;
    }

    jobject instance = _env->CallStaticObjectMethod(clazz, method_getRuntime);
    if (instance == NULL) {
        LOGE("Unable to obtain VMRuntime instance\n");
        return -1;
    }
    gIDCache.runtimeInstance = _env->NewGlobalRef(instance);

    /*
     * Register methods.
     */
    return jniRegisterNativeMethods(_env,
                "org/apache/harmony/luni/platform/OSMemory",
                gMethods, NELEM(gMethods));
}
