package test10;
import java.util.LinkedList; // 导入LinkedList类用于存储学院对象

public class SZU {
    // 使用LinkedList来存储学院对象
    private LinkedList<College> colleges;

    // 构造函数，初始化学院列表
    public SZU() {
        colleges = new LinkedList<College>();
    }

    // 添加学院到SZU对象中
    public void addCollege(College c) {
        colleges.add(c);
    }

    // 输出所有学院的名字
    public void showAllColleges() {
        System.out.print("All colleges: ");
        for(College college : colleges) {
            System.out.print(college.getName() + " ");
        }
        System.out.println(); // 换行以便后续输出不会在同一行
    }

    // 返回学院的数量
    public int getCollegeNum() {
        return colleges.size();
    }
}