package test5;

public class Test2 {
    public static void main(String[] args) {
        // 定义一个字符串s，并调用m(s)方法，统计大写字母的数量
        String s = "Hi, Good Morning";
        System.out.println(m(s));
    }

    // 统计字符串s中大写字母的个数
    public static int m(String s) {
        int count = 0;  // 计数器初始化为0
        for (int i = 0; i < s.length(); i++) {
            // 如果当前字符是大写字母，计数器加1
            if (Character.isUpperCase(s.charAt(i)))
                count++;
        }
        return count;  // 返回大写字母的个数
    }
}
