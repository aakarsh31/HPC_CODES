import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;

public class ALockTest implements Runnable {
    private static int sharedVariable = 0;
    private static final int NUM_THREADS = 4;
    private static final int NUM_INCREMENTS = 1000000;
    private static ALock lock = new ALock(NUM_THREADS);
    
    public void run() {
        for (int i = 0; i < NUM_INCREMENTS; i++) {
            lock.lock();
            try {
                sharedVariable++;
            } finally {
                lock.unlock();
            }
        }
    }
    
    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i] = new Thread(new ALockTest());
            threads[i].start();
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i].join();
        }
        System.out.println("The final value of the shared variable is: " + sharedVariable);
    }
}
