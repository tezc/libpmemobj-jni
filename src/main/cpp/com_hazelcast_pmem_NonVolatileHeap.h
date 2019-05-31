#include <libpmemobj.h>
#include <libpmem.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jni.h>

#ifndef _Included_com_hazelcast_pmem_NonVolatileHeap
#define _Included_com_hazelcast_pmem_NonVolatileHeap
#ifdef __cplusplus
extern "C" {
#endif

#define CHAR_TYPE_OFFSET 1017
TOID_DECLARE(char, CHAR_TYPE_OFFSET);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCreateHeap
  (JNIEnv *env, jobject, jstring, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeOpenHeap
  (JNIEnv*, jobject, jstring);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCloseHeap
  (JNIEnv*, jobject, jlong);

JNIEXPORT jint JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeSetRoot
  (JNIEnv *env, jobject, jlong, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeGetRoot
  (JNIEnv *env, jobject, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAlloc
  (JNIEnv*, jobject, jlong, jlong);

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeRealloc
  (JNIEnv*, jobject, jlong, jlong, jlong);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeFree
  (JNIEnv*, jobject, jlong);

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeStartTransaction
  (JNIEnv *env, jobject obj, jlong poolHandle);

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCommitTransaction
  (JNIEnv *env, jobject obj);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeEndTransaction
  (JNIEnv *env, jobject obj);

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAbortTransaction
  (JNIEnv *env, jobject obj);

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeTransactionState
  (JNIEnv *env, jobject obj);

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAddToTransaction
  (JNIEnv *env, jobject obj, jlong poolHandle, jlong address, jlong size);

#ifdef __cplusplus
}
#endif
#endif
