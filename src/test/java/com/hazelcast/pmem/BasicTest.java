package com.hazelcast.pmem;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Random;

public class BasicTest {
    private VolatileHeap heap;

    @Before
    public void open() throws IOException {
        heap = VolatileHeap.createHeap("/mnt/mem/file", 1000 * 1024 * 1024, false);
    }

    @After
    public void tearDown() {
        if (heap != null) {
            heap.close();
        }
    }

    @Test
    public void openTest() throws IOException {
        long[] ptrs = new long[10000];
        for (int i = 0; i < 10000; i++) {
            ptrs[i] = heap.allocate(new Random().nextInt(40000) + 1);
            ptrs[i] = heap.realloc(ptrs[i], new Random().nextInt(40000) + 1);
        }

        for (int i = 0; i < 10000; i++) {
            heap.free(ptrs[i]);
        }

        heap.close();
    }
}
