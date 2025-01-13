package test4;
import java.util.Random;

public class Test {
    // 生成随机数组
    public static void generateRandomArray(int[] array, int length) {
        Random rand = new Random(); // 创建随机数生成器
        for (int i = 0; i < length; i++) {
            array[i] = rand.nextInt(1000000); // 随机生成0到999999之间的数
        }
    }
    
    public static void main(String[] args) {
        QuickSort qSort = new QuickSort(); // 创建快速排序对象
        StopWatch stopWatch = new StopWatch(); // 创建秒表对象

        // 数组长度集合
        int[] sizes = {100000, 200000, 500000, 1000000, 2000000, 5000000};
        
        for (int size : sizes) {
            int[] array = new int[size]; // 初始化数组
            generateRandomArray(array, size); // 生成随机数组
            System.out.print("The array size is " + size + ".\n");
            
            // 输出数组最后五个元素
            System.out.print("The last five elements of the array before sorting: ");
            for (int i = size - 5; i < size; ++i) {
                System.out.print(array[i] + " ");
            }
            System.out.print("\n");
            
            stopWatch.start(); // 开始计时
            qSort.quickSort(array, 0, size - 1); // 执行快速排序
            stopWatch.stop(); // 停止计时
            System.out.println("The sorting takes " + stopWatch.getRunningTime() + " seconds.");
            
            // 输出排序后数组最后五个元素
            System.out.print("The last five elements of the sorted array: ");
            for (int i = size - 5; i < size; ++i) {
                System.out.print(array[i] + " ");
            }
            System.out.print("\n\n");
        }
    }
}
