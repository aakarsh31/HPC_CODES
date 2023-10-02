public class CLHLockTest {
    private static int sum = 0;
    private static CLHLock lock = new CLHLock();
    
    public static void main(String[] args) throws InterruptedException {
        Thread t1 = new Thread(() -> {
            for (int i = 0; i < 100000; i++) {
                lock.lock();
                try {
                    sum++;
                } finally {
                    lock.unlock();
                }
            }
        });
        
        Thread t2 = new Thread(() -> {
            for (int i = 0; i < 100000; i++) {
                lock.lock();
                try {
                    sum--;
                } finally {
                    lock.unlock();
                }
            }
        });
        
        t1.start();
        t2.start();
        t1.join();
        t2.join();
        
        System.out.println("Final sum: " + sum);
    }
}
