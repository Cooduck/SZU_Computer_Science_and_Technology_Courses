import java.util.*;
public class test8 {
    public static void main(String[] args) { 
        double sum = 0; 
        long fenzi = 1, fenmu = 1; // 初始化分子 fenzi 和分母 fenmu，都为 1
        // 从第1项开始，计算30项的和
        for (int i = 1; i <= 30; ++i) {
        	sum += 1.0 * fenzi / fenmu; // 将当前项的值累加到 sum 中
            fenzi += 1; // 分子递增 1
            fenmu *= 3; // 分母每次乘以 3
        }
        System.out.print("数列前30项之和为：" + sum);
    }
}

// 第八题的另一个版本
//public class test8 {
//	public static void main(String[] args) { 
//		Fraction num;
//		num = new Fraction(1L, 1L);
//		long fenzi = 1, fenmu = 1;
//		for(int i = 1; i <= 29; ++i) {
//			fenzi += 1;
//			fenmu *= 3;
//			Fraction tmp = new Fraction(fenzi, fenmu);
//			num = Fraction.add(num, tmp);
//			num.show();
//			System.out.print("\n");
//		}
//		System.out.print("数列前30项之和为：");
//		num.show();
//	}
//}
