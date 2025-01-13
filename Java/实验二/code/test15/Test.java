package test15;

public class Test {
    public static void main(String[] args) {
        // 使用父类Bird的引用创建子类对象
        Bird eagle = new Eagle();
        Bird dove = new Dove();
        Bird sparrow = new Sparrow();

        // 调用Eagle的方法
        System.out.println("Eagle:");
        eagle.flying();  // 调用老鹰的飞行方法
        eagle.nesting(); // 调用老鹰的筑巢方法
        eagle.eating();  // 调用老鹰的进食方法
        eagle.singing(); // 调用老鹰的歌唱方法
        
        // 调用Dove的方法
        System.out.println("\nDove:");
        dove.flying();  // 调用鸽子的飞行方法
        dove.nesting(); // 调用鸽子的筑巢方法
        dove.eating();  // 调用鸽子的进食方法
        dove.singing(); // 调用鸽子的歌唱方法
        
        // 调用Sparrow的方法
        System.out.println("\nSparrow:");
        sparrow.flying();  // 调用麻雀的飞行方法
        sparrow.nesting(); // 调用麻雀的筑巢方法
        sparrow.eating();  // 调用麻雀的进食方法
        sparrow.singing(); // 调用麻雀的歌唱方法
    }
}

