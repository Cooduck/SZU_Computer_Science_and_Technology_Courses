package test6;

public class HumanTest2 {
    public static void main(String[] args) {
        // 创建Chinese2实例
        Chinese2 chinsesman = new Chinese2("简校滨");
        // 创建Japanese2实例
        Japanese2 japaneseman = new Japanese2("ジャン・シャオビン");
        // 创建English2实例
        English2 englishman = new English2("Xiaobin Jian");
        
        // 调用各实例的sayHello方法
        chinsesman.sayHello();
        japaneseman.sayHello();
        englishman.sayHello();
    }
}
