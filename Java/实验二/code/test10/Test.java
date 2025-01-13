package test10;

public class Test {
    public static void main(String[] args) {
        // 创建SZU对象
        SZU szu = new SZU();

        // 添加学院
        szu.addCollege(new College("计算机与软件学院"));
        szu.addCollege(new College("电子与信息工程学院"));
        szu.addCollege(new College("机电与控制工程学院"));
        szu.addCollege(new College("建筑与城市规划学院"));

        // 调用方法获取学院名称
        szu.showAllColleges();

        // 调用方法获取学院数量
        System.out.println("College num: " + szu.getCollegeNum());
    }
}