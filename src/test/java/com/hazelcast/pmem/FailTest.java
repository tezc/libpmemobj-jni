package com.hazelcast.pmem;

import org.junit.Test;

import java.io.IOException;

public class FailTest {

    @Test(expected=OutOfMemoryError.class)
    public void test() throws IOException {
        VolatileHeap heap = VolatileHeap.openHeap("/mnt/mem/", 1000 * 1024 * 1024, false);

        heap.allocate(2000 * 1024 * 1024);
    }
}
