package com.hazelcast.pmem;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

public class FailTest {

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

    @Test(expected=OutOfMemoryError.class)
    public void test() {
        heap.allocate(2000 * 1024 * 1024);
    }
}
