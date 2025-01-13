//package test1;
//
//// 打印指定字符指定次数的任务类，实现了 Runnable 接口
//class PrintChar implements Runnable {
//    private char charToPrint; // 要打印的字符
//    private int times;        // 打印的次数
//
//    // 构造函数：传入要打印的字符和打印次数
//    public PrintChar(char c, int t) {
//        charToPrint = c;
//        times = t;
//    }
//
//    @Override
//    // 重写 run 方法：打印指定字符指定次数
//    public void run() {
//        for (int i = 0; i < times; i++) {  // 重复指定次数
//            System.out.print(charToPrint); // 打印字符
//        }
//    }
//}
//
//// 打印 1 到 n 的数字任务类，实现了 Runnable 接口
//class PrintNum implements Runnable {
//    private int lastNum; // 要打印的最大数字
//
//    // 构造函数：传入要打印的最大数字
//    public PrintNum(int n) {
//        lastNum = n;
//    }
//
//    @Override
//    // 重写 run 方法：打印 1 到 n 的数字
//    public void run() {
//        for (int i = 1; i <= lastNum; i++) {  // 从 1 打印到 lastNum
//            System.out.print(" " + i);        // 打印数字并在前加空格
//        }
//    }
//}
//
//// 主程序类
//public class TaskThreadDemo {
//    public static void main(String[] args) {
//        // 创建三个任务
//        Runnable printA = new PrintChar('a', 100);  // 打印字符 'a' 100 次
//        Runnable printB = new PrintChar('b', 100);  // 打印字符 'b' 100 次
//        Runnable print100 = new PrintNum(100);      // 打印数字 1 到 100
//
//        // 创建三个线程，并将任务传递给线程
//        Thread thread1 = new Thread(printA); 
//        Thread thread2 = new Thread(printB); 
//        Thread thread3 = new Thread(print100);
//
//        // 启动线程
//        thread1.start();  // 启动打印 'a' 的线程
//        thread2.start();  // 启动打印 'b' 的线程
//        thread3.start();  // 启动打印数字的线程
//    }
//}
