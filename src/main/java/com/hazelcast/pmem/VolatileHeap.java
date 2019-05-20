package com.hazelcast.pmem;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class VolatileHeap {
    static {
        NativeLibrary.load();
    }

    //This is approximation to overhead of libmemobj, no details are known
    //just relies on observation.
    private static final int FILE_SIZE_OVERHEAD = 10 * 1024 * 1024;

    private static final Object lock = new Object();
    private final Object closeLock = new Object();

    private final Path path;
    private final long size;
    private final long poolHandle;
    private boolean open;

    private VolatileHeap(Path path, long size, long poolHandle) {
        this.path = path;
        this.size = size;
        this.poolHandle = poolHandle;
        this.open = true;
    }

    public static VolatileHeap openHeap(String path, long size) throws IOException {
        size += FILE_SIZE_OVERHEAD;
        long poolHandle = 0;

        synchronized (lock) {
            try {
                poolHandle = nativeOpenHeap(path, size);
            } catch (IOException e) {
                Files.deleteIfExists(Paths.get(path));
                poolHandle = nativeCreateHeap(path, size);
            }

            VolatileHeap heap = new VolatileHeap(Paths.get(path), size, poolHandle);
            Runtime.getRuntime().addShutdownHook(new Thread(heap::close));

            return heap;
        }
    }

    public void close() {
        synchronized (closeLock) {
            if (open) {
                nativeCloseHeap(poolHandle);

                try {
                    Files.deleteIfExists(path);
                } catch (IOException e) {
                    //ignore
                }

                open = false;
            }
        }
    }

    public long allocate(long size) {
        return toAddress(nativeAlloc(poolHandle, size));
    }

    public long realloc(long addr, long size) {
        return toAddress(nativeRealloc(poolHandle, toHandle(addr), size));
    }

    public void free(long addr) {
        nativeFree(toHandle(addr));
    }

    public long toAddress(long handle) {
        if (handle == 0) {
            return 0;
        }

        return poolHandle + handle;
    }

    public long toHandle(long addr) {
        if (addr == 0) {
            return 0;
        }

        long handle = addr - poolHandle;
        if (handle < 0) {
            throw new IllegalArgumentException("addr is " + addr + " poolHandle is " + poolHandle);
        }
        return addr - poolHandle;
    }

    private static native long nativeCreateHeap(String path, long size) throws IOException;
    private static native long nativeOpenHeap(String path, long size) throws IOException;
    private static native void nativeCloseHeap(long poolHandle);

    private static native long nativeAlloc(long poolHandle, long size);
    private static native long nativeRealloc(long poolHandle, long addr, long size);
    private static native int nativeFree(long addr);
}