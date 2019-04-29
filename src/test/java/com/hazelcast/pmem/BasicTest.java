package com.hazelcast.pmem;

import org.junit.Test;

import java.io.IOException;

public class BasicTest {

    @Test
    public void openTest() throws IOException {
        VolatileHeap heap = VolatileHeap.openHeap("/mnt/mem/pool", 30 * 1024 * 1024);

        long ptr = heap.allocate(3000);
        heap.free(ptr);
    }
}
