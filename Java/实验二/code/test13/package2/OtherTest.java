package test13.package2; // 定义不同的包
import test13.package1.*; // 导入包中的类

public class OtherTest{
    public static void main(String[] args) {
        // DefaultClass obj = new DefaultClass(); // 编译错误，无法生成DefaultClass类对象
        
        // 私有、默认、受保护、公共成员不能访问
        // System.out.println(obj.privateMember); // 编译错误
        // .out.println("Default Member: " + obj.defaultMember); // 编译错误
        // .out.println("Protected Member: " + obj.protectedMember); // 编译错误
        // .out.println("Public Member: " + obj.publicMember); // 编译错误
    }
}
