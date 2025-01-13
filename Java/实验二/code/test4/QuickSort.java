package test4;

public class QuickSort {
    // 快速排序的主方法
    public void quickSort(int[] array, int low, int high) {
        if (low < high) {
            int pivotIndex = partition(array, low, high); // 获取基准值的最终位置
            quickSort(array, low, pivotIndex - 1); // 递归排序左半部分
            quickSort(array, pivotIndex + 1, high); // 递归排序右半部分
        }
    }

    // 分区方法，返回基准值的最终位置
    private int partition(int[] array, int low, int high) {
        int pivot = array[low]; // 选择基准值
        while (low < high) {
            while (low < high && array[high] > pivot) {
                --high; // 从右侧找到第一个小于基准值的元素
            }
            array[low] = array[high]; // 将找到的元素放到左侧
            while (low < high && array[low] <= pivot) {
                ++low; // 从左侧找到第一个大于基准值的元素
            }
            array[high] = array[low]; // 将找到的元素放到右侧
        }
        array[low] = pivot; // 将基准值放到正确位置
        return low; // 返回基准值的索引
    }
}
