package com.hazelcast.pmem;

import org.junit.Test;

import java.io.IOException;
import java.util.Random;

public class BasicTest {

    @Test
    public void openTest() throws IOException {
        VolatileHeap heap = VolatileHeap.openHeap("/mnt/mem/dasd", 1000 * 1024 * 1024, false);

        long[] ptrs = new long[10000];
        for (int i = 0; i < 10000; i++) {
            ptrs[i] = heap.allocate(new Random().nextInt(40000) + 1);
            ptrs[i] = heap.realloc(ptrs[i], new Random().nextInt(40000) + 1);
        }

        for (int i = 0; i < 10000; i++) {
            heap.free(ptrs[i]);
        }

        heap.close();


        System.out.println("dsa");
    }
}
