class Lader // 定义一个类 Lader，表示梯形
{
    double above, height; // 实例变量 above 表示上底，height 表示高
    static double bottom; // 静态变量 bottom 表示下底，所有对象共享
    
    void setAbove(double a) // 定义 setAbove 方法，设置上底的值
    {
        above = a; // 将参数 a 赋值给实例变量 above
    }

    void setBottom(double b) // 定义 setBottom 方法，设置下底的值（静态变量）
    {
        bottom = b; // 将参数 b 赋值给静态变量 bottom
    }

    double getAbove() // 定义 getAbove 方法，获取上底的值
    {
        return above; // 返回实例变量 above
    }

    double getBottom() // 定义 getBottom 方法，获取下底的值
    {
        return bottom; // 返回静态变量 bottom
    }
}

public class Example4_2 // 定义主类 Example4_2
{
    public static void main(String args[]) // 主方法，程序的入口
    {
        Lader.bottom = 60; // 通过类名 Lader 直接访问静态变量 bottom 并赋值为 60
        Lader laderOne, laderTwo; // 声明两个 Lader 类型的对象引用 laderOne 和 laderTwo
        System.out.println(Lader.bottom); // 输出静态变量 bottom 的值，60
        laderOne = new Lader(); // 创建一个 Lader 对象并赋值给 laderOne
        laderTwo = new Lader(); // 创建另一个 Lader 对象并赋值给 laderTwo
        System.out.println(laderOne.getBottom()); // 输出 laderOne 对象获取的下底 bottom 的值，60
        System.out.println(laderTwo.getBottom()); // 输出 laderTwo 对象获取的下底 bottom 的值，60
        laderOne.setAbove(11); // 设置 laderOne 对象的上底为 11
        laderTwo.setAbove(22); // 设置 laderTwo 对象的上底为 22
        laderTwo.setBottom(100); // 修改静态变量 bottom 为 100，通过 laderTwo 设置
        System.out.println(Lader.bottom); // 输出修改后的静态变量 bottom 的值，100
        System.out.println(laderOne.getAbove()); // 输出 laderOne 对象的上底，11
        System.out.println(laderTwo.getAbove()); // 输出 laderTwo 对象的上底，22
    }
}