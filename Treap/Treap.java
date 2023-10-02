import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;

public class Treap<T extends Comparable<T>> {
    private static class Node<T> {
        T key;
        int priority;
        Node<T> left;
        Node<T> right;
        Node<T> parent;

        Node(T key, int priority) {
            this.key = key;
            this.priority = priority;
        }
    }

    private final AtomicInteger size = new AtomicInteger(0);
    private final Lock lock;
    private Node<T> root;

    public Treap(Lock lock) {
        this.lock = lock;
    }

    public int size() {
        return size.get();
    }

    public boolean add(T key) {
        lock.lock();
        try {
            if (root == null) {
                root = new Node<>(key, randomPriority());
                size.incrementAndGet();
                return true;
            }
            







            Node<T> node = root;
            while (true) {
                int cmp = key.compareTo(node.key);
                if (cmp == 0) {
                    return false;
                }

                if (cmp < 0) {
                    if (node.left == null) {
                        node.left = new Node<>(key, randomPriority());
                        node.left.parent = node;
                        size.incrementAndGet();
                        bubbleUp(node.left);
                        return true;
                    }
                    node = node.left;
                } else {
                    if (node.right == null) {
                        node.right = new Node<>(key, randomPriority());
                        node.right.parent = node;
                        size.incrementAndGet();
                        bubbleUp(node.right);
                        return true;
                    }
                    node = node.right;
                }
            }
        } finally {
            lock.unlock();
        }
    }
    


    

