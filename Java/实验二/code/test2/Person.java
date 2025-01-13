package test2;

public class Person {
    private String name;    // 姓名
    private double weight;   // 体重
    private double height;   // 身高

    // 构造函数，用于初始化Person对象
    public Person(String name, double weight, double height) {
        this.name = name;    // 初始化姓名
        this.weight = weight; // 初始化体重
        this.height = height; // 初始化身高
    }

    // 重写Object类的toString方法
    @Override
    public String toString() {
        return "name: " + name + ", weight: " + weight + "kg, height: " + height + "m";
    }

    // 主方法用于测试Person类
    public static void main(String[] args) {
        Person person = new Person("Xiaobin Jian", 75.0, 1.80); // 创建Person对象
        System.out.println(person); // 调用重写的toString方法输出信息
    }
}