package test9;
import java.util.*;

// 显示器类
class Monitor1 {
    private int Id;        // 显示器ID
    private String size;   // 显示器尺寸
    private double price;  // 显示器价格

    // 构造函数
    public Monitor1(int Id, String size, double price) {
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
}

// 价格比较器（实现Comparator接口）
class PriceComparator implements Comparator<Monitor1> {
    @Override
    public int compare(Monitor1 monitor1, Monitor1 monitor2) {
        double price1 = monitor1.getPrice();
        double price2 = monitor2.getPrice();
        // 根据价格进行比较
        if(price1 < price2) return -1; // price1 小于 price2
        else if (price1 > price2) return 1; // price1 大于 price2
        else return 0; // 价格相同
    }
}

// 主测试类
public class Test1 {
    public static void main(String[] args) {
        // 创建一个TreeMap，按价格排序的监视器集合
        TreeMap<Monitor1, String> monitors = new TreeMap<>(new PriceComparator());

        // 添加显示器信息
        monitors.put(new Monitor1(0, "24 inches", 528.88), "Monitor0");
        monitors.put(new Monitor1(1, "21 inches", 123.77), "Monitor1");
        monitors.put(new Monitor1(2, "23 inches", 799.99), "Monitor2");
        monitors.put(new Monitor1(3, "27 inches", 1500.00), "Monitor3");
        monitors.put(new Monitor1(4, "25 inches", 1099.99), "Monitor4");
        monitors.put(new Monitor1(5, "22 inches", 599.99), "Monitor5");
        monitors.put(new Monitor1(6, "24 inches", 676.78), "Monitor6");
        monitors.put(new Monitor1(7, "28 inches", 1120.14), "Monitor7");

        // 输出所有显示器信息，按价格升序排列
        for (Map.Entry<Monitor1, String> entry : monitors.entrySet()) {
            System.out.println(entry.getKey() + " -> " + entry.getValue());
        }
    }
}
