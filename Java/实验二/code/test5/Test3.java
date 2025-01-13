package test5;

public class Test3 {
    public static void main(String[] args) {
        // 定义一个字符串s，并用StringBuilder包装该字符串
        String s = "Java";
        StringBuilder builder = new StringBuilder(s);
        
        // 调用change方法尝试修改s和builder
        change(s, builder);

        // 打印字符串s，观察s是否被修改
        System.out.println(s);  // 输出"Java"
        
        // 打印StringBuilder对象builder，观察builder是否被修改
        System.out.println(builder);  // 输出"Java and HTML"
    }

    // 修改字符串s和StringBuilder对象builder
    private static void change(String s, StringBuilder builder) {
        s = s + " and HTML";  // 由于Java的字符串是不可变的，s只在局部改变，不影响外部的s
        builder.append(" and HTML");  // StringBuilder是可变的，修改会影响外部
    }
}
