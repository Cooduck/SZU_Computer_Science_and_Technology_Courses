package test1;

//定义一个抽象类Vehicle，表示车辆的共通特性
abstract class Vehicle {
 private String name; // 车辆名称
 private int speed;   // 车辆速度
 private int wheel;   // 车辆轮子数

 // 构造函数，用于初始化车辆的名称、速度和轮子数
 public Vehicle(String name, int speed, int wheel) {
     this.name = name; // 初始化名称
     this.speed = speed; // 初始化速度
     this.wheel = wheel; // 初始化轮子数
 }

 // 获取车辆名称
 public String getName() {
     return name;
 }

 // 获取车辆速度
 public int getSpeed() {
     return speed;
 }

 // 获取车辆轮子数
 public int getWheel() {
     return wheel;
 }

 // 获取车辆的基本信息
 public void info() {
     System.out.print("This Vehicle is a " + name + ", "
             + "it has " + wheel + " wheels and can move at a speed of "
             + speed + " km/h.\n");
 }

 // 定义一个抽象方法honk，子类必须实现该方法以提供不同的鸣笛声
 public abstract void honk();
}