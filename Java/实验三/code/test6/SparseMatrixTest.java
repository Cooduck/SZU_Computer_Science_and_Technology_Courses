package test6;

import java.util.Map;
import java.util.HashMap;
import java.util.Scanner;

// 表示矩阵中非零元素的位置类
class Pos {
    public int row, col;

    // 构造函数
    public Pos(int row, int col) {
        this.row = row;
        this.col = col;
    }

    // 重写 equals 方法，用于对象比较
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Pos pos = (Pos) obj;
        return row == pos.row && col == pos.col;
    }

    // 重写 hashCode 方法，用于将对象作为键放入 HashMap
    @Override
    public int hashCode() {
        return Integer.toString(row).hashCode() ^ Integer.toString(col).hashCode();
    }
}

// 表示稀疏矩阵的类
class SparseMatrix {
    private int rows, cols; // 矩阵的行数和列数
    private Map<Pos, Integer> matrix; // 用于存储非零元素的哈希表

    // 构造函数
    public SparseMatrix(int rows, int cols) {
        this.rows = rows;
        this.cols = cols;
        this.matrix = new HashMap<>();
    }

    // 设置矩阵中的非零元素
    public void setElement(int row, int col, int value) {
        if (value != 0) {
            matrix.put(new Pos(row, col), value);
        }
    }

    // 获取矩阵中的元素值（零元素默认为 0）
    public Integer getElement(int row, int col) {
        if (matrix.get(new Pos(row, col)) != null)
            return matrix.get(new Pos(row, col));
        else
            return 0;
    }

    // 实现稀疏矩阵的加法运算
    public SparseMatrix add(SparseMatrix other) throws IllegalArgumentException {
        if (this.rows != other.rows || this.cols != other.cols) {
            throw new IllegalArgumentException("矩阵大小不一致，无法进行加法运算！");
        }

        SparseMatrix result = new SparseMatrix(this.rows, this.cols);

        // 遍历当前矩阵的非零元素并添加到结果矩阵
        for (Map.Entry<Pos, Integer> entry : this.matrix.entrySet()) {
            Pos pos = entry.getKey();
            int thisValue = entry.getValue();
            result.setElement(pos.row, pos.col, thisValue);
        }

        // 遍历另一个矩阵的非零元素并添加到结果矩阵
        for (Map.Entry<Pos, Integer> entry : other.matrix.entrySet()) {
            Pos pos = entry.getKey();
            int otherValue = entry.getValue();
            int tmp = result.getElement(pos.row, pos.col);
            if (tmp == 0) {
                result.setElement(pos.row, pos.col, otherValue);
            } else {
                int sum = tmp + otherValue;
                if (sum == 0) {
                    result.matrix.remove(new Pos(pos.row, pos.col)); // 如果加和为零则移除元素
                } else {
                    result.setElement(pos.row, pos.col, sum);
                }
            }
        }
        return result;
    }

    // 实现稀疏矩阵的乘法运算
    public SparseMatrix multiply(SparseMatrix other) throws IllegalArgumentException {
        if (this.cols != other.rows) {
            throw new IllegalArgumentException("矩阵维度不匹配，无法进行乘法运算！");
        }

        SparseMatrix result = new SparseMatrix(this.rows, other.cols);

        // 遍历当前矩阵的非零元素
        for (Map.Entry<Pos, Integer> entry : this.matrix.entrySet()) {
            Pos pos = entry.getKey();
            int thisValue = entry.getValue();

            // 遍历第二个矩阵的列，计算对应位置的乘积
            for (int col = 0; col < other.cols; ++col) {
                int otherValue = other.getElement(pos.col, col);
                int res = thisValue * otherValue;
                int tmp = result.getElement(pos.row, col);

                if (tmp == 0) {
                    result.setElement(pos.row, col, res);
                } else {
                    int sum = tmp + res;
                    if (sum == 0) {
                        result.matrix.remove(new Pos(pos.row, col));
                    } else {
                        result.setElement(pos.row, col, sum);
                    }
                }
            }
        }
        return result;
    }

    // 打印稀疏矩阵（输出完整矩阵形式）
    public void printMatrix() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                System.out.print(getElement(i, j) + " ");
            }
            System.out.println();
        }
    }
}

// 测试类
public class SparseMatrixTest {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        try {
            // 读取第一个矩阵
            System.out.println("请输入第一个矩阵的行数和列数：");
            int rows1 = scanner.nextInt();
            int cols1 = scanner.nextInt();
            SparseMatrix matrix1 = new SparseMatrix(rows1, cols1);

            System.out.println("请输入第一个矩阵的元素（非零元素的行号-列号-值），停止输入则输入“0 0 0”：");
            for (int i = 0; i < rows1 * cols1; i++) {
                int row = scanner.nextInt();
                int col = scanner.nextInt();
                int value = scanner.nextInt();
                if (value == 0) break;
                matrix1.setElement(row, col, value);
            }

            // 读取第二个矩阵
            System.out.println("请输入第二个矩阵的行数和列数：");
            int rows2 = scanner.nextInt();
            int cols2 = scanner.nextInt();
            SparseMatrix matrix2 = new SparseMatrix(rows2, cols2);

            System.out.println("请输入第二个矩阵的元素（非零元素的行号-列号-值），停止输入则输入“0 0 0”：");
            for (int i = 0; i < rows2 * cols2; i++) {
                int row = scanner.nextInt();
                int col = scanner.nextInt();
                int value = scanner.nextInt();
                if (value == 0) break;
                matrix2.setElement(row, col, value);
            }

            // 执行矩阵加法
            System.out.println("两个矩阵的加法结果为：");
            SparseMatrix sumMatrix = matrix1.add(matrix2);
            sumMatrix.printMatrix();

            // 执行矩阵乘法
            System.out.println("两个矩阵的乘法结果为：");
            SparseMatrix productMatrix = matrix1.multiply(matrix2);
            productMatrix.printMatrix();

        } catch (IllegalArgumentException e) {
            System.out.println("错误：" + e.getMessage());
        } catch (Exception e) {
            System.out.println("输入错误：" + e.getMessage());
        } finally {
            scanner.close();
        }
    }
}
