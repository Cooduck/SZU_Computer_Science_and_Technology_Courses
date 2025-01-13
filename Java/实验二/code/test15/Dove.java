package test15;

// Dove类继承自Bird，并实现了所有抽象方法
class Dove extends Bird {
    @Override
    void flying() {
        System.out.println("Dove is flying peacefully."); // 实现鸽子的飞行行为
    }

    @Override
    void nesting() {
        System.out.println("Dove builds its nest in trees."); // 实现鸽子的筑巢行为
    }

    @Override
    void eating() {
        System.out.println("Dove pecks at grains."); // 实现鸽子的进食行为
    }

    @Override
    void singing() {
        System.out.println("Dove coos softly."); // 实现鸽子的歌唱行为
    }
}