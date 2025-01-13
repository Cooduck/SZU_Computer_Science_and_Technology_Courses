package test15;

// Eagle类继承自Bird，并实现了所有抽象方法
public class Eagle extends Bird {
    @Override
    void flying() {
        System.out.println("Eagle is soaring high in the sky."); // 实现老鹰的飞行行为
    }

    @Override
    void nesting() {
        System.out.println("Eagle builds its nest on mountain cliffs."); // 实现老鹰的筑巢行为
    }

    @Override
    void eating() {
        System.out.println("Eagle is hunting for prey."); // 实现老鹰的捕猎行为
    }

    @Override
    void singing() {
        System.out.println("Eagle screeches loudly."); // 实现老鹰的嘶叫行为
    }
}