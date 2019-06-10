package com.hazelcast.pmem;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class VolatileHeap {
    static {
        NativeLibrary.load();
    }

    private final Object closeLock = new Object();

    private final Path path;
    private final long heapHandle;
    private boolean open;

    private VolatileHeap(Path path, long heapHandle) {
        this.path = path;
        this.heapHandle = heapHandle;
        this.open = true;
    }

    public static synchronized VolatileHeap openHeap(String path, long size, boolean validate) throws IOException {
        Path dir = Paths.get(path);
        Files.createDirectories(dir);
        long poolHandle = nativeCreateHeap(path, size, validate);

        return new VolatileHeap(dir, poolHandle);
    }

    public void close() {
        synchronized (closeLock) {
            if (open) {
                nativeCloseHeap(heapHandle);
                open = false;
            }
        }
    }

    public long allocate(long size) {
        return nativeAlloc(heapHandle, size);
    }

    public long realloc(long addr, long size) {
        return nativeRealloc(heapHandle, addr, size);
    }

    public void free(long addr) {
        nativeFree(heapHandle, addr);
    }

    private static native long nativeCreateHeap(String path, long size, boolean validate) throws IOException;
    private static native void nativeCloseHeap(long poolHandle);
    private static native long nativeAlloc(long poolHandle, long size);
    private static native long nativeRealloc(long poolHandle, long addr, long size);
    private static native int nativeFree(long poolHandle, long addr);
}
