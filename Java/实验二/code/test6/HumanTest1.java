package test6;

public class HumanTest1 {
    public static void main(String[] args) {
        // 创建一个Chinese1对象，姓名为"简校滨"
        Chinese1 chinsesman = new Chinese1("简校滨");
        // 创建一个Japanese1对象，姓名为"ジャン・シャオビン"
        Japanese1 japaneseman = new Japanese1("ジャン・シャオビン");
        // 创建一个English1对象，姓名为"Xiaobin Jian"
        English1 englishman = new English1("Xiaobin Jian");
        
        // 调用Chinese1对象的sayHello方法，输出问候
        chinsesman.sayHello();
        // 调用Japanese1对象的sayHello方法，输出问候
        japaneseman.sayHello();
        // 调用English1对象的sayHello方法，输出问候
        englishman.sayHello();
    }
}
