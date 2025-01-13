package test1;

// 定义Bike类，继承自Vehicle类
public class Bike extends Vehicle {
    // 构造函数，初始化Bike对象
    public Bike(String name, int speed, int wheel) {
        super(name, speed, wheel); // 调用父类构造函数
    }

    // 实现抽象方法honk，输出Bike特有的鸣笛声
    public void honk() {
        System.out.println("dingding~");
    }

    // main方法用于测试Bike类
    public static void main(String[] args) {
        Vehicle bike = new Bike("RX200", 40, 2); // 创建Bike对象
        bike.info(); // 输出车辆信息
        bike.honk(); // 调用鸣笛方法
    }
}
