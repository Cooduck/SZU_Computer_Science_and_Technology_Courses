package test16;

// 实现Computable接口的FourDimensionalVector类
public class FourDimensionalVector implements Computable {
    // 实现向量相加
    @Override
    public double[] add(double[] v1, double[] v2) {
        double[] result = new double[4];  // 存储结果的向量
        for (int i = 0; i < 4; i++) {  // 对应分量相加
            result[i] = v1[i] + v2[i];
        }
        return result;
    }

    // 实现向量相减
    @Override
    public double[] minus(double[] v1, double[] v2) {
        double[] result = new double[4];
        for (int i = 0; i < 4; i++) {  // 对应分量相减
            result[i] = v1[i] - v2[i];
        }
        return result;
    }

    // 实现向量对应元素相乘
    @Override
    public double[] dot(double[] v1, double[] v2) {
        double[] result = new double[4];
        for (int i = 0; i < 4; i++) {  // 对应分量相乘
            result[i] = v1[i] * v2[i];
        }
        return result;
    }
}