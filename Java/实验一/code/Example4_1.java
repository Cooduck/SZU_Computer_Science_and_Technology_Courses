class Circle // 定义一个类 Circle
{
    double radius; // 定义一个成员变量 radius，用于存储圆的半径
    
    double getArea() // 定义一个成员方法 getArea，用于计算圆的面积
    {
        double area = 3.14 * radius * radius; // 使用公式 π*r^2 计算圆的面积
        return area; // 返回计算出的面积
    }
}

public class Example4_1 // 定义主类 Example4_1
{
    public static void main(String args[]) // 主方法，程序的入口
    {
        Circle circle; // 声明一个 Circle 类型的变量 circle
        circle = new Circle(); // 实例化一个 Circle 对象，并将其引用赋值给变量 circle
        circle.radius = 1; // 设置圆的半径为 1
        double area = circle.getArea(); // 调用 getArea 方法，计算圆的面积并赋值给变量 area
        System.out.println(area); // 输出计算出的面积
    }
}
