package com.hazelcast.pmem;

import org.junit.Test;

import java.io.IOException;

public class SizeTest {
    @Test(expected = IOException.class)
    public void minimumHeapSizeTest() throws IOException {
        VolatileHeap.createHeap("/mnt/mem/file", 100);
    }
}
