package test14.package2;

import test14.package1.*;

public class OtherTest{
    public static void main(String[] args) {
        PublicClass obj = new PublicClass();

        // 私有和默认成员不能访问
        // /System.out.println(obj.privateMember);  // 编译错误
        // System.out.println(obj.defaultMember);  // 编译错误
        // System.out.println(obj.protectedMember);  // 编译错误

        // 公共成员可以访问
        System.out.println("Public Member: " + obj.publicMember);
    }
}
