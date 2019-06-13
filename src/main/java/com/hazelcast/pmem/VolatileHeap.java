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

    public static synchronized VolatileHeap createHeap(String path, long size) throws IOException {
        Path filePath = Paths.get(path);
        long heapHandle;

        try {
            heapHandle = nativeCreateHeap(path, size);
        } catch (IOException e) {
            try {
                Files.deleteIfExists(filePath);
            } catch (IOException e1) {
                //ignore
            }

            throw e;
        }

        return new VolatileHeap(filePath, heapHandle);
    }

    public void close() throws IOException {
        synchronized (closeLock) {
            if (open) {
                nativeCloseHeap(heapHandle);
                try {
                    Files.deleteIfExists(path);
                } catch (IOException e) {
                    System.err.println("Failed to delete file at : " + path);
                }
                open = false;
            }
        }
    }

    public boolean isPmem() {
        return nativeIsPmem(heapHandle);
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

    private static native long nativeCreateHeap(String path, long size) throws IOException;
    private static native void nativeCloseHeap(long heapHandle) throws IOException;
    private static native boolean nativeIsPmem(long heapHandle);
    private static native long nativeAlloc(long heapHandle, long size);
    private static native long nativeRealloc(long heapHandle, long addr, long size);
    private static native void nativeFree(long heapHandle, long addr);
}
