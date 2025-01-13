package test7;

import java.util.*;

// 计算器类，用于计算菜品总价格
public class Calculator {
    Map<String, Double> priceList; // 存储菜品及其对应价格的映射

    // 构造方法，初始化价格表
    Calculator() {
        priceList = new HashMap<String, Double>();
        priceList.put("白贝汤", 48.8);
        priceList.put("北京烤鸭", 299.1);
        priceList.put("西芹炒肉", 32.9);
        priceList.put("回锅肉", 58.8);
        priceList.put("酸菜鱼", 79.0);
        priceList.put("铁板牛柳", 42.1);
        priceList.put("蒜炒菜心", 28.8);
        priceList.put("生蚝一打", 88.8);
    }
    
    // 计算给定菜单的总价格
    void calPrice(LinkedList<String> menu) {
        double total = 0.0; // 初始化总价

        // 遍历菜单中的每个菜品
        for (String dish : menu) {
            // 检查菜品是否在价格表中
            if (priceList.containsKey(dish)) {
                // 累加该菜品的价格
                total += priceList.get(dish);
            } else {
                // 如果菜品不在价格表中，输出提示信息
                System.out.println("菜单中没有找到: " + dish);
            }
        }

        // 输出计算出的总价格，保留两位小数
        System.out.printf("菜单总价格：%.2f元\n", total);
    }

    public static void main(String[] args) {
        // 创建计算器对象
        Calculator calculator = new Calculator();

        // 创建一个包含菜单菜品的链表
        LinkedList<String> menu = new LinkedList<>();
        menu.add("北京烤鸭"); // 添加菜品
        menu.add("酸菜鱼");
        menu.add("西芹炒肉");
        menu.add("铁板牛柳");

        // 计算并输出菜单总价格
        calculator.calPrice(menu);
    }
}
