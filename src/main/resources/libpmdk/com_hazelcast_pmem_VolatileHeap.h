#include <jni.h>

#ifndef _Included_com_hazelcast_pmem_VolatileHeap
#define _Included_com_hazelcast_pmem_VolatileHeap


JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeCreateHeap
  (JNIEnv*, jobject, jstring, jlong);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeCloseHeap
  (JNIEnv*, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeIsPmem
  (JNIEnv*, jobject, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeAlloc
  (JNIEnv*, jobject, jlong, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeRealloc
  (JNIEnv*, jobject, jlong, jlong, jlong);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeFree
  (JNIEnv*, jobject, jlong, jlong);

#endif
