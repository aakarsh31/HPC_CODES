import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MCSLockTest {
    private static final int THREAD_COUNT = 10;
    private static final int NUM_INCREMENTS = 1000000;
    private static int myCounter = 0;
    private static MCSLock myLock = new MCSLock();

    public static void main(String[] args) throws InterruptedException {
        Thread[] myThreads = new Thread[THREAD_COUNT];
        for (int i = 0; i < THREAD_COUNT; i++) {
            myThreads[i] = new Thread(() -> {
                for (int j = 0; j < NUM_INCREMENTS; j++) {
                    myLock.lock();
                    try {
                        myCounter++;
                    } finally {
                        myLock.unlock();
                    }
                }
            });
            myThreads[i].start();
        }
        for (int i = 0; i < THREAD_COUNT; i++) {
            myThreads[i].join();
        }
        System.out.println("Counter value: " + myCounter);
    }
}
    
