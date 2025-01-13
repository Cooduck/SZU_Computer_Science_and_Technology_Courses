package test11;

// 引入cn.edu包中的所有类
import cn.edu.*;

public class Test {
    public static void main(String[] args) {
        // 创建SZU类的实例
        SZU szu = new SZU();

        // 添加多个学院到SZU中
        szu.addCollege(new College("计算机与软件学院"));
        szu.addCollege(new College("电子与信息工程学院"));
        szu.addCollege(new College("机电与控制工程学院"));
        szu.addCollege(new College("建筑与城市规划学院")); 

        // 调用方法显示所有学院的名称
        szu.showAllColleges();

        // 获取并输出当前学院的数量
        System.out.println("College num: " + szu.getCollegeNum());
    }
}
