package test15;

// Sparrow类继承自Bird，并实现了所有抽象方法
class Sparrow extends Bird {
    @Override
    void flying() {
        System.out.println("Sparrow flutters through the air."); // 实现麻雀的飞行行为
    }

    @Override
    void nesting() {
        System.out.println("Sparrow builds a small nest in bushes."); // 实现麻雀的筑巢行为
    }

    @Override
    void eating() {
        System.out.println("Sparrow is eating seeds."); // 实现麻雀的进食行为
    }

    @Override
    void singing() {
        System.out.println("Sparrow chirps cheerfully."); // 实现麻雀的鸣叫行为
    }
}

