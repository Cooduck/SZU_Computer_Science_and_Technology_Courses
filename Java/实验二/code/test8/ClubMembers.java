package test8;

import java.util.*;

// 社团成员类
public class ClubMembers {
    public static void main(String[] args) {
        // 创建A社团成员集合
        Set<String> clubA = new HashSet<String>();
        clubA.add("张三"); // 添加成员
        clubA.add("李四");
        clubA.add("赵六");

        // 创建B社团成员集合
        Set<String> clubB = new HashSet<String>();
        clubB.add("李四"); // 添加成员
        clubB.add("王五");
        clubB.add("陈七");

        // 输出A社团成员
        System.out.println("A社团成员：" + clubA);

        // 输出B社团成员
        System.out.println("B社团成员：" + clubB);

        // 求两个社团的交集（同时参加两个社团的人）
        Set<String> commonMembers = new HashSet<String>(clubA); // 创建交集集合，初始化为A社团成员
        commonMembers.retainAll(clubB); // 保留仅在clubB中的元素，计算交集
        System.out.println("同时参加A和B社团的成员：" + commonMembers);
    }
}
