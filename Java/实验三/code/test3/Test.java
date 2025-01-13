package test3;

import java.lang.Thread;

public class Test {
    
    // 用于控制线程交替运行的标志
    private static boolean numberPrinterRun = true;
    
    // 用于线程间同步的锁对象
    private static final Object lock = new Object();
    
    public static void main(String[] args) {
        // 创建并启动两个线程：数字打印线程和字符打印线程
        Thread numberPrinter = new Thread(new NumberPrinter(), "NumberPrinter");
        Thread charPrinter = new Thread(new CharPrinter(), "CharPrinter");
        
        numberPrinter.start(); // 启动数字打印线程
        charPrinter.start();   // 启动字符打印线程
    }
    
    // 数字打印线程的任务
    static class NumberPrinter implements Runnable {
        @Override
        public void run() {
            for (int i = 1; i <= 26; ++i) { // 循环打印1到26的数字
                synchronized (lock) { // 加锁以保证线程安全
                    if (!numberPrinterRun) { // 检查是否该轮到数字打印线程运行
                        try {
                            lock.wait(); // 如果不是，释放锁并等待
                        } catch (InterruptedException e) {
                            Thread.currentThread().interrupt(); // 恢复线程中断状态
                        }
                    }
                }
                System.out.print(i); // 打印数字
                numberPrinterRun = false; // 标志切换到字符打印线程运行
                synchronized (lock) {
                    lock.notify(); // 通知字符打印线程可以运行
                }
            }
        }
    }
    
    // 字符打印线程的任务
    static class CharPrinter implements Runnable {
        @Override
        public void run() {
            for (int i = 1; i <= 26; ++i) { // 循环打印A到Z的字符
                synchronized (lock) { // 加锁以保证线程安全
                    if (numberPrinterRun) { // 检查是否该轮到字符打印线程运行
                        try {
                            lock.wait(); // 如果不是，释放锁并等待
                        } catch (InterruptedException e) {
                            Thread.currentThread().interrupt(); // 恢复线程中断状态
                        }
                    }
                }
                System.out.print((char) (i - 1 + 'A')); // 打印字符
                numberPrinterRun = true; // 标志切换到数字打印线程运行
                synchronized (lock) {
                    lock.notify(); // 通知数字打印线程可以运行
                }
            }
        }
    }
}
