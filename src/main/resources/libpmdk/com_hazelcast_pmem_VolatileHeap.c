#include "com_hazelcast_pmem_VolatileHeap.h"

#include "libvmem.h"
#include "libpmem.h"

#include <stdlib.h>

struct volatile_heap {
    void* pmemaddr;
    size_t mapped_len;
    VMEM* vmp;
};


void throw_OOM(JNIEnv* env, size_t size)
{
    char errmsg[256];

    jclass exClass = (*env)->FindClass(env, "java/lang/OutOfMemoryError");

    sprintf(errmsg, "Failed to allocate %lu bytes!", size);
    (*env)->ThrowNew(env, exClass, errmsg);
}

void throw_io_exception(JNIEnv* env, const char* msg)
{
    jclass exClass = (*env)->FindClass(env, "java/io/IOException");
    (*env)->ThrowNew(env, exClass, msg);
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeCreateHeap
  (JNIEnv *env, jobject obj, jstring path, jlong size, jboolean validate)
{
    size_t mapped_len = 0;
    void* pmemaddr;
    int is_pmem;
    struct volatile_heap* heap = NULL;
    int validate_requested = (validate == JNI_TRUE);
    const char* native_string = (*env)->GetStringUTFChars(env, path, 0);

    pmemaddr = pmem_map_file(native_string, (size_t)size,
                             PMEM_FILE_CREATE,
                             0666, &mapped_len, &is_pmem);
    if (pmemaddr == NULL) {
        throw_io_exception(env, pmem_errormsg());
        return (jlong) NULL;
    }

    if (validate_requested && !is_pmem) {
        pmem_unmap(pmemaddr, mapped_len);
        throw_io_exception(env, "Given path is not persistent memory!");
        return (jlong) NULL;
    }

    VMEM* vmp = vmem_create_in_region(pmemaddr, mapped_len);
    if (vmp == NULL) {
        pmem_unmap(pmemaddr, mapped_len);
        throw_io_exception(env, vmem_errormsg());
        return (jlong) NULL;
    }

    heap = (struct volatile_heap*) malloc(sizeof(struct volatile_heap));
    heap->pmemaddr = pmemaddr;
    heap->mapped_len = mapped_len;
    heap->vmp = vmp;

    return (long) heap;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeCloseHeap
  (JNIEnv *env, jobject obj, jlong heap)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) heap;

    vmem_delete(volatile_heap->vmp);

    int rc = pmem_unmap(volatile_heap->pmemaddr, volatile_heap->mapped_len);
    if (rc != 0) {
        free(volatile_heap);
        throw_io_exception(env, pmem_errormsg());
        return;
    }

    free(volatile_heap);
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeAlloc
  (JNIEnv *env, jobject obj, jlong heap, jlong size)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) heap;

    void* p = vmem_malloc(volatile_heap->vmp, (size_t) size);
    if (p == NULL) {
        throw_OOM(env, (size_t) size);
        return (jlong) NULL;
    }

    return (jlong) p;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeRealloc
  (JNIEnv *env, jobject obj, jlong heap, jlong address, jlong size)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) heap;

    void* p = vmem_realloc(volatile_heap->vmp, (void*) address, (size_t) size);
    if (p == NULL) {
        throw_OOM(env, (size_t) size);
        return (jlong) NULL;
    }

    return (jlong) p;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeFree
  (JNIEnv *env, jobject obj, jlong heap, jlong address)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) heap;
    vmem_free(volatile_heap->vmp, (void*) address);
}
