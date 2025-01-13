import java.util.*;
public class test7 {
    public static void main(String[] args) { 
        // 创建一个布尔数组，用于标记1到600之间的数字是否为素数，若为素数，则为true
        boolean[] isprime = new boolean[601];
        
        // 初始化数组，将2到600的标记设为true，表示默认它们都是素数
        Arrays.fill(isprime, 1, 600, true);
        
        for(int i = 2; i <= 600; ++i) {
            // 如果当前数字是素数
            if(isprime[i]) {
                // 如果当前数字的平方大于600，超出范围，则不需要继续标记
                if( i * i > 600) {
                    continue;
                }
                // 从当前数字的平方开始，标记所有它的倍数为非素数
                for(int j = i * i; j <= 600; j += i) {
                    isprime[j] = false;
                }
            }
        }
        
        // 计数器，用于计算素数的数量
        int cnt = 0;
        for(int i = 1; i <= 600; ++i) {
            // 如果当前数字是素数，输出并计数器+1
            if(isprime[i]) {
                System.out.print(i + " ");
                ++cnt;
                if(cnt % 30 == 0) {
                    System.out.print("\n");
                }
            }
        }
        
        // 输出1到600之间素数的总数
        System.out.print("\n1——600的素数总数为：" + cnt);
    }
}