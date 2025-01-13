package test13.package1; // 定义包

public class Test {
    public static void main(String[] args) {
        DefaultClass obj = new DefaultClass(); // 创建DefaultClass的实例
        
        // 私有成员不能访问，下面的代码将导致编译错误
        // System.out.println(obj.privateMember); // 编译错误
        
        // 默认、受保护和公共成员可以访问
        System.out.println("Default Member: " + obj.defaultMember); // 可以访问默认成员
        System.out.println("Protected Member: " + obj.protectedMember); // 可以访问受保护成员
        System.out.println("Public Member: " + obj.publicMember); // 可以访问公共成员
    }
}
