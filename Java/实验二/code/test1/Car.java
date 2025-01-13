package test1;

// 定义Car类，继承自Vehicle类
public class Car extends Vehicle {
    // 构造函数，初始化Car对象
    public Car(String name, int speed, int wheel) {
        super(name, speed, wheel); // 调用父类构造函数
    }

    // 实现抽象方法honk，输出Car特有的鸣笛声
    public void honk() {
        System.out.println("bibi~");
    }

    // main方法用于测试Car类
    public static void main(String[] args) {
        Vehicle car = new Car("su7", 120, 4); // 创建Car对象
        car.info(); // 输出车辆信息
        car.honk(); // 调用鸣笛方法
    }
}
