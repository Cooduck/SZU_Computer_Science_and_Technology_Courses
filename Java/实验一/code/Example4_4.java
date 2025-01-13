//class Lader // 定义一个类 Lader，表示梯形
//{
//    double above, bottom, height; // 定义三个成员变量：上底（above）、下底（bottom）、高（height）
//    
//    Lader(){} // 定义无参构造方法，用于创建一个没有初始值的 Lader 对象
//    
//    Lader(double a, double b, double h) // 定义带参构造方法，传入上底、下底和高的初始值
//    {
//        above = a; // 初始化上底
//        bottom = b; // 初始化下底
//        height = h; // 初始化高度
//    }
//
//    public void setAbove(double a) // 定义 setAbove 方法，设置上底的值
//    {
//        above = a; // 将参数 a 赋值给上底 above
//    }
//
//    public void setBottom(double b) // 定义 setBottom 方法，设置下底的值
//    {
//        bottom = b; // 将参数 b 赋值给下底 bottom
//    }
//
//    public void setHeight(double h) // 定义 setHeight 方法，设置高的值
//    {
//        height = h; // 将参数 h 赋值给高 height
//    }
//
//    double computeArea() // 定义 computeArea 方法，计算梯形的面积
//    {
//        return (above + bottom) * height / 2.0; // 使用公式 ((上底 + 下底) * 高 / 2) 计算梯形面积
//    }
//}
//
//public class Example4_4 // 定义主类 Example4_4
//{
//    public static void main(String args[]) // 主方法，程序的入口
//    {
//        double area1 = 0, area2 = 0; // 定义两个变量，用于存储两个梯形的面积
//        Lader laderOne, laderTwo; // 声明两个 Lader 类型的变量 laderOne 和 laderTwo
//        laderOne = new Lader(); // 使用无参构造方法创建一个 Lader 对象
//        laderTwo = new Lader(10, 88, 20); // 使用带参构造方法创建另一个 Lader 对象
//        laderOne.setAbove(16); // 设置 laderOne 对象的上底为 16
//        laderOne.setBottom(26); // 设置 laderOne 对象的下底为 26
//        laderOne.setHeight(100); // 设置 laderOne 对象的高为 100
//        laderTwo.setAbove(300); // 设置 laderTwo 对象的上底为 300
//        laderTwo.setBottom(500); // 设置 laderTwo 对象的下底为 500
//        area1 = laderOne.computeArea(); // 计算 laderOne 对象的面积并赋值给 area1
//        area2 = laderTwo.computeArea(); // 计算 laderTwo 对象的面积并赋值给 area2
//        System.out.println(area1); // 输出 laderOne 的面积 = (16 + 26) * 100 / 2 = 2100
//        System.out.println(area2); // 输出 laderTwo 的面积 = (300 + 500) * 20 / 2 = 8000
//    }
//}