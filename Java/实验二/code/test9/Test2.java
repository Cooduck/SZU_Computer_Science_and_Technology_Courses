package test9;
import java.util.*;

// 显示器类实现Comparable接口
class Monitor2 implements Comparable<Monitor2> {
    private int Id;        // 显示器ID
    private String size;   // 显示器尺寸
    private double price;  // 显示器价格

    // 构造函数
    public Monitor2(int Id, String size, double price) {
        this.Id = Id;
        this.size = size;
        this.price = price;
    }
    
    public int getId() {
        return Id;
    }
    
    public String getSize() {
        return size;
    }

    public double getPrice() {
        return price;
    }

    @Override
    public String toString() {
        return "Monitor Id = " + Id + ", size=" + size + ", price=" + price;
    }
    
    // 实现compareTo方法，按价格升序排序
    @Override
    public int compareTo(Monitor2 other) {
        return Double.compare(this.price, other.price);  // 使用Double类的compare方法进行比较
    }
}

// 主测试类
public class Test2 {
    public static void main(String[] args) {
        // 创建一个TreeMap，自动根据Comparable接口的实现排序
        TreeMap<Monitor2, String> monitors = new TreeMap<>();

        // 添加显示器信息
        monitors.put(new Monitor2(0, "24 inches", 528.88), "Monitor0");
        monitors.put(new Monitor2(1, "21 inches", 123.77), "Monitor1");
        monitors.put(new Monitor2(2, "23 inches", 799.99), "Monitor2");
        monitors.put(new Monitor2(3, "27 inches", 1500.00), "Monitor3");
        monitors.put(new Monitor2(4, "25 inches", 1099.99), "Monitor4");
        monitors.put(new Monitor2(5, "22 inches", 599.99), "Monitor5");
        monitors.put(new Monitor2(6, "24 inches", 676.78), "Monitor6");
        monitors.put(new Monitor2(7, "28 inches", 1120.14), "Monitor7");

        // 输出所有显示器信息，按价格升序排列
        for (Map.Entry<Monitor2, String> entry : monitors.entrySet()) {
            System.out.println(entry.getKey() + " -> " + entry.getValue());
        }
    }
}
