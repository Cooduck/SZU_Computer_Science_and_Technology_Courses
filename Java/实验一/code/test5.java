import java.util.Scanner; 
public class test5 {
    // 定义一个静态方法 cal_money，用于根据用电量计算电费
    public static double cal_money(int num) {
        double money = 0; // 初始化电费为 0
        if (num <= 50) { // 如果用电量小于或等于 50 度
            money = num * 0.56; 
        } else if (num <= 220) { // 如果用电量在 51 度到 220 度之间
            money = 50 * 0.56 + (num - 50) * 0.59; 
        } else { // 如果用电量超过 220 度
            money = 50 * 0.56 + 170 * 0.59 + (num - 220) * 0.66; 
        }
        return money;
    }

    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        
        // 循环 10 次，接收 10 个用电量输入并计算电费
        for (int i = 0; i < 10; ++i) {
            System.out.print("输入用电量(度): ");
            int num = scan.nextInt(); // 读取用户输入的整数用电量
            double money = cal_money(num); // 调用 cal_money 方法，计算电费
            System.out.printf("电费（元）: %.2f \n", money);
        }
        
        scan.close();
    }
}
