package test16;

// 定义接口Computable，规定向量的基本操作
public interface Computable {
    // 定义抽象方法，表示向量相加
    double[] add(double[] v1, double[] v2);

    // 定义抽象方法，表示向量相减
    double[] minus(double[] v1, double[] v2);

    // 定义抽象方法，表示向量对应元素相乘
    double[] dot(double[] v1, double[] v2);
}