import java.util.*;
public class test4 {
    public static void main(String[] args) {
    	// 生成100×100的数组
        double[][] array = new double[100][100];
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                array[i][j] = Math.random();	// 对元素随机赋值
            }
        }

        long startTime = System.nanoTime();	// 记录起始时间
        PriorityQueue<Double> topK = new PriorityQueue<Double>(5);	// 维护一个最小堆

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                double value = array[i][j];
                
                // 如果堆中元素少于5，则直接添加元素，
                // 否则该元素需要大于堆顶元素，才能被添加入堆中
                if (topK.size() < 5) {
                    topK.offer(value);
                } else if (value > topK.peek()) {
                    topK.poll();
                    topK.offer(value);
                }
            }
        }

        // 用一个数组存最大的5个元素，并排序
        ArrayList<Double> result = new ArrayList<Double>(topK);
        Collections.sort(result, Collections.reverseOrder());

        long endTime = System.nanoTime();	// 记录结束时间
        long duration = (endTime - startTime);	// 记录总耗时

        System.out.println("Top 5 elements from the array:");
        for (double val : result) {
            System.out.println(val);
        }

        System.out.println("Time: " + duration / 1000000.0f + "ms.");
    }
}