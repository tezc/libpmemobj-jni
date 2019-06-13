#include "com_hazelcast_pmem_VolatileHeap.h"

#include "libvmem.h"
#include "libpmem.h"

#include <stdlib.h>

struct volatile_heap {
    void* mapped_addr;
    size_t mapped_len;
    int is_pmem;
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
  (JNIEnv* env, jobject obj, jstring path, jlong size)
{
    char errmsg[256];
    size_t mapped_len = 0;
    void* mapped_addr;
    int is_pmem;
    struct volatile_heap* heap = NULL;
    const char* file_path = (*env)->GetStringUTFChars(env, path, 0);

    if ((size_t) size <= VMEM_MIN_POOL) {
        sprintf(errmsg, "Heap size must be greater than %lu", VMEM_MIN_POOL);
        throw_io_exception(env, errmsg);
        return (jlong) NULL;
    }

    mapped_addr = pmem_map_file(file_path, (size_t) size,
                                PMEM_FILE_CREATE,
                                0666, &mapped_len, &is_pmem);
    if (mapped_addr == NULL) {
        throw_io_exception(env, pmem_errormsg());
        return (jlong) NULL;
    }

    VMEM* vmp = vmem_create_in_region(mapped_addr, mapped_len);
    if (vmp == NULL) {
        pmem_unmap(mapped_addr, mapped_len);
        throw_io_exception(env, vmem_errormsg());
        return (jlong) NULL;
    }

    heap = (struct volatile_heap*) malloc(sizeof(struct volatile_heap));
    heap->mapped_addr = mapped_addr;
    heap->mapped_len = mapped_len;
    heap->is_pmem = is_pmem;
    heap->vmp = vmp;

    return (long) heap;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeCloseHeap
  (JNIEnv* env, jobject obj, jlong handle)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) handle;

    vmem_delete(volatile_heap->vmp);

    int rc = pmem_unmap(volatile_heap->mapped_addr, volatile_heap->mapped_len);
    if (rc != 0) {
        free(volatile_heap);
        throw_io_exception(env, pmem_errormsg());
        return;
    }

    free(volatile_heap);
}

JNIEXPORT jboolean JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeIsPmem
  (JNIEnv* env, jobject obj, jlong handle)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) handle;

    return volatile_heap->is_pmem != 0;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeAlloc
  (JNIEnv *env, jobject obj, jlong handle, jlong size)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) handle;

    void* p = vmem_malloc(volatile_heap->vmp, (size_t) size);
    if (p == NULL) {
        throw_OOM(env, (size_t) size);
        return (jlong) NULL;
    }

    return (jlong) p;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeRealloc
  (JNIEnv* env, jobject obj, jlong handle, jlong address, jlong size)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) handle;

    void* p = vmem_realloc(volatile_heap->vmp, (void*) address, (size_t) size);
    if (p == NULL) {
        throw_OOM(env, (size_t) size);
        return (jlong) NULL;
    }

    return (jlong) p;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_VolatileHeap_nativeFree
  (JNIEnv* env, jobject obj, jlong heap, jlong address)
{
    struct volatile_heap* volatile_heap = (struct volatile_heap*) heap;
    vmem_free(volatile_heap->vmp, (void*) address);
}
