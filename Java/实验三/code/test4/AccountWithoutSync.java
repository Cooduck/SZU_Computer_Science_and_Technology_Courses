package test4;

public class AccountWithoutSync { // 定义一个没有同步机制的账户类

    private static Account account = new Account(); // 创建一个账户实例，初始余额为0
    
    private static final Object lock = new Object(); // 定义一个锁对象，用于线程同步

    public static void main(String[] args) { // 主方法，程序的入口点
        int threadCount = 100; // 线程数量，每个线程向账户存入1分钱
        
        Thread[] threads = new Thread[threadCount]; // 创建线程数组

        // 创建线程，每个线程负责执行 AddAPennyTask 任务
        for (int i = 0; i < threadCount; i++) {
            threads[i] = new Thread(new AddAPennyTask(), "Thread-" + i);
        }

        // 启动所有线程
        for (int i = 0; i < threadCount; i++) {
            threads[i].start();
        }

        // 使用 join() 等待所有线程完成
        for (int i = 0; i < threadCount; i++) {
            try {
                threads[i].join(); // 等待线程完成任务
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt(); // 捕获中断异常并重设中断标志
            }
        }

        // 打印账户余额
        System.out.println("账户余额为 " + account.getBalance());
    }

    // 定义一个内部类，用于向账户中添加1分钱的任务
    private static class AddAPennyTask implements Runnable {
        public void run() {
            account.deposit(1); // 调用账户的存款方法，存入1分钱
        }
    }

    // 定义账户的内部类
    private static class Account {
        private int balance = 0; // 账户的初始余额为0

        // 获取账户余额的方法
        public int getBalance() {
            return balance; // 返回当前余额
        }

        // 存款方法，存入指定金额
        public void deposit(int amount) {
            // 同步块，确保多个线程访问时的线程安全
            synchronized (lock) {
                balance = balance + amount; // 更新账户余额
                System.out.println(Thread.currentThread().getName() + 
                		" 存入: " + 1 + "，当前余额: " + account.getBalance());
            }

        }
    }
}
