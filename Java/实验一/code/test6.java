import java.util.*;
public class test6 {
    // 定义一个静态方法 reverse，用于将正整数逆序
    public static String reverse(long num) {
        String ans = ""; // 初始化结果字符串为空
        while(num != 0) { // 当 num 不等于 0 时执行循环
            ans = ans + (num % 10); // 取 num 的个位数字并追加到结果字符串中
            num /= 10; // 去掉个位数字，处理下一位
        }
        return ans; 
    }

    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        
        // 循环 10 次，接收 10 个正整数输入并输出逆序结果
        for (int i = 0; i < 10; ++i) {
            System.out.printf("第%d次输入正整数: ", i+1);
            long num = scan.nextLong(); // 读取用户输入的正整数
            String reverse_num = reverse(num); // 调用 reverse 方法，逆序该正整数
            System.out.println("逆序打印结果: " + reverse_num); // 输出逆序结果
        }
        
        scan.close();
    }
}
