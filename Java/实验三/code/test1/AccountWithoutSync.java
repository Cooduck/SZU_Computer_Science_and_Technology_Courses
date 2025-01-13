package test1;

import java.util.concurrent.*; // 导入java.util.concurrent包，用于线程池和线程管理

public class AccountWithoutSync{ // 定义一个没有同步机制的账户类

    private static Account account = new Account(); // 创建一个账户实例，初始余额为0

    public static void main(String[] args){ // 主方法，程序的入口点

        ExecutorService executor = Executors.newCachedThreadPool(); // 创建一个可缓存线程池，它会根据需要创建新线程

        // 创建并启动100个线程，每个线程向账户中存入1分钱
        for(int i = 0; i < 100; i++){
            executor.execute(new AddAPennyTask());
        }

        executor.shutdown(); // 关闭线程池，不再接受新任务，已提交的任务继续执行

        // 等待直到所有任务都执行完毕
        while(!executor.isTerminated()){
            // 循环等待，直到线程池中的所有任务都执行完毕
        }

        System.out.println("What is balance?" + account.getBalance()); // 打印账户余额
    }

    // 定义一个内部类，用于向账户中添加1分钱的任务
    private static class AddAPennyTask implements Runnable{
        public void run(){
            account.deposit(1); // 调用账户的存款方法，存入1分钱
        }
    }

    // 定义账户的内部类
    private static class Account{
        private int balance = 0; // 账户的初始余额为0

        public int getBalance(){ // 获取账户余额的方法
            return balance; // 返回当前余额
        }

        public void deposit(int amount){ // 存款方法，存入指定金额
            int newBalance = balance + amount; // 计算新的余额

            // 故意添加的延迟，用于放大数据损坏问题，使其容易被观察到
            try{
                Thread.sleep(5); // 让当前线程暂停5毫秒
            } catch(InterruptedException ex){
                // 如果线程在休眠期间被中断，捕获异常
            }

            balance = newBalance; // 更新账户余额
        }
    }
}