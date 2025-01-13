package test16;
import java.util.*;  // 用于Arrays工具类

// 测试类
public class Test {
    public static void main(String[] args) {
        // 定义两个四维向量
        double[] v1 = {5, 2, 1, 8};
        double[] v2 = {3, -1, 0, -4};

        // 创建FourDimensionalVector的实例
        FourDimensionalVector vector = new FourDimensionalVector();

        // 向量相加
        double[] sum = vector.add(v1, v2);
        System.out.println("Add: " + Arrays.toString(sum));  // 输出相加结果

        // 向量相减
        double[] difference = vector.minus(v1, v2);
        System.out.println("Minus: " + Arrays.toString(difference));  // 输出相减结果

        // 向量对应元素相乘
        double[] product = vector.dot(v1, v2);
        System.out.println("Dot: " + Arrays.toString(product));  // 输出点乘结果
    }
}