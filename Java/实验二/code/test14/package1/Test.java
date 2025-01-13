package test14.package1;

public class Test {
    public static void main(String[] args) {
        PublicClass obj = new PublicClass();

        // 私有成员无法访问
        // System.out.println(obj.privateMember); // 编译错误

        // 默认、受保护和公共成员可以访问
        System.out.println("Default Member: " + obj.defaultMember);
        System.out.println("Protected Member: " + obj.protectedMember);
        System.out.println("Public Member: " + obj.publicMember);
    }
}
