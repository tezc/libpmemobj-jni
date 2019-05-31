package com.hazelcast.pmem;

import org.junit.Test;
import org.openjdk.jmh.annotations.*;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import sun.misc.Unsafe;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Random;
import java.util.concurrent.*;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MICROSECONDS)
@State(Scope.Benchmark)
@Fork(value = 1, warmups = 0)
@Warmup(iterations = 2)
@Measurement(iterations = 5)
@Threads(6)
public class TransactionJMHTest {
    private final int ALLOCATION_SIZE = 8192;
    public static Unsafe UNSAFE;

    static {
        try {
            java.lang.reflect.Field f = Unsafe.class.getDeclaredField("theUnsafe");
            f.setAccessible(true);
            UNSAFE = (Unsafe)f.get(null);
        }
        catch (Exception e) {
            throw new RuntimeException("Unable to initialize UNSAFE.");
        }
    }

    private NonVolatileHeap heap;

    private long ptr1;
    private long ptr2;
    private long ptr3;
    private long ptr4;
    private long ptr5;

    private Random random = new Random();

    @Setup
    public void setup() throws IOException {
        String pmemDir = System.getProperty("pmemDir");
        if (pmemDir == null) {
            pmemDir = "/mnt/mem/pool";
        }

        System.out.println("Pmem dir : " + pmemDir);

        Files.deleteIfExists(Paths.get(pmemDir));
        heap = NonVolatileHeap.createHeap(pmemDir, 1000 * 1024 * 1024);
        ptr1 = heap.allocate(ALLOCATION_SIZE);
        ptr2 = heap.allocate(ALLOCATION_SIZE);
        ptr3 = heap.allocate(ALLOCATION_SIZE);
        ptr4 = heap.allocate(ALLOCATION_SIZE);
        ptr5 = heap.allocate(ALLOCATION_SIZE);
    }

    @TearDown
    public void tearDown() {
        if (heap != null) {
            heap.close();
        }
    }

    @Benchmark
    public void testSmall() {
        final int MOD_SIZE = 32;

        heap.addToTransaction(ptr1, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr1, MOD_SIZE);

        heap.addToTransaction(ptr2, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr2, MOD_SIZE);

        heap.addToTransaction(ptr3, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr3, MOD_SIZE);

        heap.addToTransaction(ptr4, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr4, MOD_SIZE);

        int rc = heap.commitTransaction();
        if (rc != 2) {
            throw new IllegalArgumentException("Failed with " + rc);
        }
    }

    @Benchmark
    public void testSmallWithoutTransaction() {
        final int MOD_SIZE = 32;

        UNSAFE.copyMemory(ptr5, ptr1, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr2, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr3, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr4, MOD_SIZE);
    }

    @Benchmark
    public void testLarge() {
        final int MOD_SIZE = 4096;

        heap.addToTransaction(ptr1, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr1, MOD_SIZE);

        heap.addToTransaction(ptr2, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr2, MOD_SIZE);

        heap.addToTransaction(ptr3, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr3, MOD_SIZE);

        heap.addToTransaction(ptr4, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr4, MOD_SIZE);

        int rc = heap.commitTransaction();
        if (rc != 2) {
            throw new IllegalArgumentException("Failed with " + rc);
        }
    }

    @Benchmark
    public void testLargeWithoutTransaction() {
        final int MOD_SIZE = 4096;

        UNSAFE.copyMemory(ptr5, ptr1, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr2, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr3, MOD_SIZE);
        UNSAFE.copyMemory(ptr5, ptr4, MOD_SIZE);
    }

    @Test
    public void perfTest() throws RunnerException {
        Options opt = new OptionsBuilder()
                .include(TransactionJMHTest.class.getSimpleName())
                .build();

        new Runner(opt).run();
    }
}
