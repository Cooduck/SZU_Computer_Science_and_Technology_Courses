//import java.util.*;
//
//// 用于第8题的另一个版本的分数类
//public class Fraction{
//	long fenzi, fenmu;
//	
//	public Fraction(long fenzi, long fenmu) {
//		this.fenzi = fenzi;
//		this.fenmu = fenmu;
//	}
//	
//	public void show() {
//		System.out.print(fenzi + "/" + fenmu);
//	}
//	
//	// 返回两数的最大公约数
//	static long gcd(long a, long b){
//		if(a < b) {
//			long tmp = a;
//			a = b;
//			b = tmp;
//		}
//	    if(b==0){
//	        return a;
//	    }else{
//	    	 return gcd(b, a%b);
//	    }
//	}
//	
//	// 返回两数的最小公倍数
//	static long lcm(long a, long b){
//		
//	    return a*b/gcd(a, b);
//	}
//	
//	// 返回两个分数相加结果
//	public static Fraction add(Fraction num1, Fraction num2) {
//		long newFenmu = lcm(num1.fenmu, num2.fenmu);
//		long tmp1 = num1.fenzi * (newFenmu / num1.fenmu);
//		long tmp2 = num2.fenzi * (newFenmu / num2.fenmu);
//		long newFenzi = tmp1 + tmp2;
//		long gcdResult = gcd(newFenzi, newFenmu);
//		return new Fraction(newFenzi / gcdResult, newFenmu / gcdResult);
//	}
//}