package test5;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;

public class DateDifferenceCalculator {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        // 定义日期格式，用户输入必须符合该格式
        SimpleDateFormat inputFormat = new SimpleDateFormat("yyyy年MM月dd日HH时mm分ss秒");
        Date[] dates = new Date[5]; // 用于存储用户输入的5个日期

        // 循环读取用户输入的5个日期，并验证格式与数值的合法性
        for (int i = 0; i < 5; ++i) {
            try {
                System.out.print("请输入第 " + (i + 1) + " 个日期：");
                String dateString = scanner.nextLine();

                // 检查日期格式与数值是否合法
                if (!isValidDate(dateString)) {
                    System.out.println("输入格式或数值错误！请重新输入!");
                    --i; // 减少计数器以便用户重新输入
                    continue;
                }

                // 解析用户输入为 Date 对象
                dates[i] = inputFormat.parse(dateString);
            } catch (ParseException e) {
                // 捕获解析错误并提示重新输入
                System.out.println("日期格式错误！请按照“××××年××月××日××时××分××秒”格式输入!");
                System.out.println("请重新输入!");
                --i; // 减少计数器以便用户重新输入
            }
        }

        // 计算并输出相邻日期之间的时间差
        System.out.println("\n相邻日期之间的时间间隔：");
        for (int i = 0; i < 4; i++) {
            long diffInMillis = dates[i + 1].getTime() - dates[i].getTime(); // 计算时间差（毫秒）
            long hours = TimeUnit.MILLISECONDS.toHours(diffInMillis); // 转换为小时
            long minutes = TimeUnit.MILLISECONDS.toMinutes(diffInMillis) % 60; // 剩余分钟
            long seconds = TimeUnit.MILLISECONDS.toSeconds(diffInMillis) % 60; // 剩余秒数
            long days = hours / 24; // 转换为天数
            hours = hours % 24; // 计算剩余小时

            // 输出相邻日期之间的时间间隔
            System.out.println("第 " + (i + 1) + " 和第 " + (i + 2) + " 个日期之间的间隔："
                    + days + "日" + hours + "时" + minutes + "分" + seconds + "秒");
        }

        scanner.close(); // 关闭输入流
    }

    // 验证日期格式与数值是否合法
    public static boolean isValidDate(String dateString) {
        // 定义正则表达式验证格式：年份不限位数，月份、日期、时间范围严格限制
        String regex = "^\\d+年(0[1-9]|1[0-2])月(0[1-9]|[12]\\d|3[01])日([01]\\d|2[0-3])时([0-5]\\d)分([0-5]\\d)秒$";
        if (!dateString.matches(regex)) {
            return false; // 格式不匹配直接返回 false
        }

        // 提取数值进行进一步验证
        int year = Integer.parseInt(dateString.substring(0, dateString.indexOf("年")));
        int month = Integer.parseInt(dateString.substring(dateString.indexOf("年") + 1, dateString.indexOf("月")));
        int day = Integer.parseInt(dateString.substring(dateString.indexOf("月") + 1, dateString.indexOf("日")));
        int hour = Integer.parseInt(dateString.substring(dateString.indexOf("日") + 1, dateString.indexOf("时")));
        int minute = Integer.parseInt(dateString.substring(dateString.indexOf("时") + 1, dateString.indexOf("分")));
        int second = Integer.parseInt(dateString.substring(dateString.indexOf("分") + 1, dateString.indexOf("秒")));

        // 调用 isValidDay 方法检查日期数值的逻辑合法性
        return isValidDay(year, month, day, hour, minute, second);
    }

    public static boolean isValidDay(int year, int month, int day, int hour, int minute, int second) {
        // 定义每月的天数，默认不是闰年
        int[] daysInMonth = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        // 判断闰年，修改 2 月的天数为 29 天
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            daysInMonth[1] = 29;
        }

        // 检查日期和时间范围是否合法
        return day >= 1 && day <= daysInMonth[month - 1] && hour >= 0 && hour <= 23 &&
               minute >= 0 && minute <= 59 && second >= 0 && second <= 59;
    }
}
