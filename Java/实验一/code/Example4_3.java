class Tom // 定义一个类 Tom
{
    final int MAX = 100; // 定义实例常量 MAX，值为 100，final 表示常量不可改变
    final static int MIN = 20; // 定义静态常量 MIN，值为 20，static 表示类级别共享，final 表示不可改变
}

public class Example4_3 // 定义主类 Example4_3
{
    public static void main(String args[]) // 主方法，程序的入口
    {
        System.out.println(Tom.MIN); // 直接通过类名访问静态常量 MIN，输出 20
        
        // System.out.println(Tom.MAX); // 错误：MAX 是实例常量，不能通过类名直接访问
        
        Tom cat = new Tom(); // 创建一个 Tom 类的实例 cat
        System.out.println(cat.MAX); // 通过对象 cat 访问实例常量 MAX，输出 100
    }
}
