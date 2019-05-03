package com.hazelcast.pmem;

import org.junit.Test;

import java.io.IOException;
import java.util.Random;

public class BasicTest {

    @Test
    public void openTest() throws IOException {
        VolatileHeap heap = VolatileHeap.openHeap("/mnt/mem/pool", 1000 * 1024 * 1024);

        long[] ptrs = new long[10000];
        for (int i = 0; i < 10000; i++) {
            ptrs[i] = heap.allocate(new Random().nextInt(40000));
        }

        for (int i = 0; i < 10000; i++) {
            heap.free(ptrs[i]);
        }

        heap.close();


    }
}
