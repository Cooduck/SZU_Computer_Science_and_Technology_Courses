package test12; // 定义包test12

// 定义类I_Love_CSSE
class I_Love_CSSE {
    // 定义一个方法用于输出字符串
    public void printMessage() {
        System.out.println("I love CSSE");  // 打印信息
    }
}

public class Test {
    public static void main(String[] args) {
        // 创建I_Love_CSSE类的实例
        I_Love_CSSE message = new I_Love_CSSE();
        // 调用方法输出信息
        message.printMessage(); // 输出"I love CSSE"
    }
}
