package test1;

import java.util.concurrent.*;

// 一个表示打印指定字符指定次数的任务类
class PrintChar implements Runnable {
    private char charToPrint; // 要打印的字符
    private int times; // 要打印的次数

    // 构造方法，初始化要打印的字符和次数
    public PrintChar(char c, int t) {
        charToPrint = c; // 将指定的字符赋值给 charToPrint
        times = t; // 将指定的次数赋值给 times
    }

    @Override
    // 重写 run() 方法，以定义任务执行时的行为
    public void run() {
        // 循环指定次数
        for (int i = 0; i < times; i++) {
            System.out.print(charToPrint); // 每次打印一次字符
        }
    }
}

// 一个表示打印从 1 到指定数值的数字序列的任务类
class PrintNum implements Runnable {
    private int lastNum; // 要打印的最后一个数字

    // 构造方法，初始化要打印的最后一个数字
    public PrintNum(int n) {
        lastNum = n; // 将指定的值赋值给 lastNum
    }

    @Override
    // 重写 run() 方法，以定义任务执行时的行为
    public void run() {
        // 循环打印从 1 到 lastNum 的数字
        for (int i = 1; i <= lastNum; i++) {
            System.out.print(" " + i); // 每次打印一个数字，前面加一个空格
        }
    }
}

public class ExecutorDemo {
    public static void main(String[] args) {
        // 创建一个固定大小的线程池，最多包含三个线程
        ExecutorService executor = Executors.newFixedThreadPool(3);

        // 向线程池提交任务
        executor.execute(new PrintChar('a', 100)); // 提交一个打印 'a' 100 次的任务
        executor.execute(new PrintChar('b', 100)); // 提交一个打印 'b' 100 次的任务
        executor.execute(new PrintNum(100)); // 提交一个打印从 1 到 100 的任务

        // 关闭线程池，等待所有任务执行完成
        executor.shutdown();
    }
}