package test5;

public class Test1 {
    public static void main(String[] args) {
        // 检查字符串"Hi, ABC, good"是否完全匹配"ABC "，结果为false
        System.out.println("Hi, ABC, good".matches("ABC "));
        
        // 使用正则表达式".*ABC.*"匹配，允许"ABC"出现在字符串的任何位置，结果为true
        System.out.println("Hi, ABC, good".matches(".*ABC.*"));
        
        // 使用replaceAll方法将字符串中的逗号替换为"#"，结果为"A#B#C"
        System.out.println("A,B,C".replaceAll(",", "#"));
        
        // 使用replaceAll方法将字符串中的逗号或分号替换为"#"，结果为"A#B#C"
        System.out.println("A,B;C".replaceAll("[,;]", "#"));

        // 使用split方法将字符串以逗号或分号分割，并输出分割后的每个元素
        String[] tokens = "A,B;C".split("[,;]");
        for (int i = 0; i < tokens.length; i++)
            // 依次输出A B C
            System.out.print(tokens[i] + " ");
    }
}

