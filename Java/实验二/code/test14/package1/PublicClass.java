package test14.package1; // 定义包

public class PublicClass {
    // 各种访问权限的成员变量
    private String privateMember = "Private Member"; // 私有成员，只有在本类中可以访问
    String defaultMember = "Default Member";  // 默认访问权限，包内可访问
    protected String protectedMember = "Protected Member"; // 受保护成员，包内和子类可访问
    public String publicMember = "Public Member"; // 公共成员，任何地方都可访问
}
