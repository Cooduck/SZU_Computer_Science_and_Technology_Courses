package test2;

import java.util.Random;

import java.lang.Thread;

public class Test {
    // 用于共享随机数的变量
    private static int randomNumber;
    
    // 用于线程间的同步
    private static final Object lock = new Object();
    
    // 控制线程停止
    private static Boolean stop = false;
    
    public static void main(String[] args) {
        // 创建一个生成随机数的线程
        Thread randomNumberGenerator = new Thread(new RandomNumberGenerator());
        // 创建一个读取随机数并判断奇偶性的线程
        Thread randomNumberReader = new Thread(new RandomNumberReader());
        
        // 启动随机数生成线程
        randomNumberGenerator.start();
        // 启动随机数读取线程
        randomNumberReader.start();
        
        try {
            // 主线程等待 20 秒
            Thread.sleep(20000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        
        // 设置停止标志，通知两个线程结束
        stop = true;
    }
    
    // 定义随机数生成器线程的类
    static class RandomNumberGenerator implements Runnable {
        private Random random = new Random(); // 随机数生成器
        
        @Override
        public void run() {
            while (!stop) { // 当未收到停止信号时循环运行
                int num = random.nextInt(1000); // 生成一个 0-999 的随机数
                
                synchronized (lock) { // 使用锁确保线程间的同步
                    randomNumber = num; // 更新共享的随机数
                    System.out.println("RandomNumberGenerator生成新的随机数：" + randomNumber);
                }
                
                try {
                    Thread.sleep(500); // 暂停 500 毫秒
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            System.out.println("线程RandomNumberGenerator已停止");
        }
    }
    
    // 定义随机数读取线程的类
    static class RandomNumberReader implements Runnable {
        
        @Override
        public void run() {
            while (!stop) { // 当未收到停止信号时循环运行
                int checkNum;
                
                synchronized (lock) { // 使用锁确保读取操作与写入操作同步
                    checkNum = randomNumber; // 读取当前共享的随机数
                    System.out.println("RandomNumberReader读取随机数：" + randomNumber);
                    if (checkNum % 2 == 1) { // 判断随机数是奇数还是偶数
                        System.out.println("随机数 " + randomNumber + " 是奇数");
                    } else {
                        System.out.println("随机数 " + randomNumber + " 是偶数");
                    }
                }
                
                try {
                    Thread.sleep(1500); // 暂停 1500 毫秒
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            System.out.println("线程RandomNumberReader已停止");
        }
    }
}
