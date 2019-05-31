#include "com_hazelcast_pmem_NonVolatileHeap.h"

const char *nonvolatile_layout_name = "com_hazelcast_pmem_nonvolatile_heap";

void native_throw_OOM(JNIEnv *env)
{
    char className[50] = "java/lang/OutOfMemoryError";
    jclass exClass = env->FindClass(className);

    char errmsg[250];
    strcpy(errmsg, pmemobj_errormsg());
    env->ThrowNew(exClass, errmsg);
}

void native_throw_exception(JNIEnv *env)
{
    char className[50] = "java/lang/IllegalArgumentException";
    jclass exClass = env->FindClass(className);

    char errmsg[250];
    strcpy(errmsg, pmemobj_errormsg());
    env->ThrowNew(exClass, errmsg);
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCreateHeap
  (JNIEnv *env, jobject obj, jstring path, jlong size)
{
    const char* native_string = env->GetStringUTFChars(path, 0);

    PMEMobjpool *pool = pmemobj_open(native_string, nonvolatile_layout_name);
    if (pool != NULL) {
        pmemobj_close(pool);
        return 0;
    }

    pool = pmemobj_create(native_string, nonvolatile_layout_name, (size_t) size, S_IRUSR | S_IWUSR);

    env->ReleaseStringUTFChars(path, native_string);

    return (long) pool;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeOpenHeap
  (JNIEnv *env, jobject obj, jstring path)
{
    const char* native_string = env->GetStringUTFChars(path, 0);

    PMEMobjpool *pool = pmemobj_open(native_string, nonvolatile_layout_name);

    env->ReleaseStringUTFChars(path, native_string);

    return (long) pool;
}

JNIEXPORT jint JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeSetRoot
  (JNIEnv *env, jobject obj, jlong poolHandle, jlong val)
{
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    int ret = 0;
    TX_BEGIN(pool) {
        long *root_address = (long *)pmemobj_direct(pmemobj_root(pool, 8));
        pmemobj_tx_add_range_direct((const void *)root_address, 8);
        *root_address = val;
    } TX_ONABORT {
        ret = -1;
    } TX_END

    return ret;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeGetRoot
  (JNIEnv *env, jobject obj, jlong poolHandle)
{
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    return (jlong)*(long *)pmemobj_direct(pmemobj_root(pool, 8));
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCloseHeap
  (JNIEnv *env, jobject obj, jlong poolHandle)
{
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    pmemobj_close(pool);
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAlloc
  (JNIEnv *env, jobject obj, jlong poolHandle, jlong size)
{
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    PMEMoid bytes = OID_NULL;

    int rc = pmemobj_alloc(pool, &bytes, (size_t)size, 0, NULL, NULL);
    if (rc == -1) {
        native_throw_OOM(env);
    }

    return bytes.off;
}

JNIEXPORT jlong JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeRealloc
  (JNIEnv *env, jobject obj, jlong poolHandle, jlong address, jlong size)
{
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    PMEMoid bytes = pmemobj_oid((const void*)address);

    int rc = pmemobj_realloc(pool, &bytes, (size_t)size, 0);
    if (rc == -1) {
        native_throw_OOM(env);
    }

    return bytes.off;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeFree
  (JNIEnv *env, jobject obj, jlong address)
{
    PMEMoid oid = pmemobj_oid((const void*)address);
    TOID(char) bytes;

    TOID_ASSIGN(bytes, oid);
    POBJ_FREE(&bytes);
}







JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeStartTransaction
  (JNIEnv *env, jobject obj, jlong poolHandle)
{
    int result = 0;
    PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
    int ret = pmemobj_tx_begin(pool, NULL, TX_PARAM_NONE);
    if (ret) {
        pmemobj_tx_end();
        result = -1;
    }
    return result;
}

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeCommitTransaction
  (JNIEnv *env, jobject obj)
{
    int rc = 0;
    if (pmemobj_tx_stage() == TX_STAGE_NONE) {
        rc = -1;
    }
    if (pmemobj_tx_stage() == TX_STAGE_WORK) {
        pmemobj_tx_commit();
        rc = 1;
    }
    if (pmemobj_tx_stage() == TX_STAGE_ONCOMMIT | pmemobj_tx_stage() == TX_STAGE_ONABORT) {
        pmemobj_tx_end();
        return 2;
    }

    return rc;
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeEndTransaction
  (JNIEnv *env, jobject obj)
{
    pmemobj_tx_end();
}

JNIEXPORT void JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAbortTransaction
  (JNIEnv *env, jobject obj)
{
    if (pmemobj_tx_stage() == TX_STAGE_WORK) pmemobj_tx_abort(0);
    if (pmemobj_tx_stage() == TX_STAGE_ONABORT) pmemobj_tx_end();
}

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeTransactionState
  (JNIEnv *env, jobject obj)
{
    switch (pmemobj_tx_stage())
    {
        case TX_STAGE_NONE: return 1;
        case TX_STAGE_WORK: return 2;
        case TX_STAGE_ONCOMMIT: return 3;
        case TX_STAGE_ONABORT: return 4;
        case TX_STAGE_FINALLY: return 5;
        default: return 0;
    }
}

JNIEXPORT int JNICALL Java_com_hazelcast_pmem_NonVolatileHeap_nativeAddToTransaction
  (JNIEnv *env, jobject obj, jlong poolHandle, jlong address, jlong size)
{
    int result = -1;
    int stage = pmemobj_tx_stage();
    if (stage == TX_STAGE_NONE) {
        PMEMobjpool *pool = (PMEMobjpool*)poolHandle;
        int begin_result = pmemobj_tx_begin(pool, NULL, TX_PARAM_NONE);
        if (begin_result) {
            result = -2;
            pmemobj_tx_end();
            native_throw_exception(env);
        }
        else {
            int rc = pmemobj_tx_add_range_direct((const void *)address, (size_t)size);
            if (rc != 0) {
                native_throw_exception(env);
            }
            return 2;
        }
    }
    else if (stage == TX_STAGE_WORK) {
        pmemobj_tx_add_range_direct((const void *)address, (size_t)size);
        result = 2;
    }

    return result;
}


