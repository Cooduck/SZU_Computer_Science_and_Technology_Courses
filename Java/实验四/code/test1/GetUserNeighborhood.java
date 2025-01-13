// 导入所需的Java类库
package test1;

import java.io.*;
import java.util.*;
import java.util.concurrent.*;

public class GetUserNeighborhood {
    // 存储每个用户的商品集合
    private static Map<String, Set<String>> userProducts = new HashMap<>();
    // 输入和输出文件的路径
    static String inputFile = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test1\\review.txt";
    static String outputFile = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test1\\userNeighborhood.txt";

    // 计算 Jaccard 相似度
    private static double calJaccard(Set<String> userA, Set<String> userB) {
        // 计算两个集合的交集
        Set<String> intersection = new HashSet<>(userA);
        intersection.retainAll(userB);
        // 计算两个集合的并集
        Set<String> union = new HashSet<>(userA);
        union.addAll(userB);
        // 返回交集大小除以并集大小，即Jaccard相似度
        return (double) intersection.size() / union.size();
    }

    // 读取 review.txt 文件，构建用户-商品映射
    private static void readReviews() throws IOException {
        try (BufferedReader br = new BufferedReader(new FileReader(inputFile))) {
            String line;
            String userId = "";
            String productId = "";

            while ((line = br.readLine()) != null) {
                // 解析 userId 和 productId
                userId = line.split(";")[0].split(":")[1].trim();
                productId = line.split(";")[1].split(":")[1].trim();

                // 确保每个用户的评论商品集合已存在
                userProducts.putIfAbsent(userId, new HashSet<>());

                // 将商品ID加入到该用户的集合中
                userProducts.get(userId).add(productId);
            }
        }
    }

    // 计算每个用户与其他用户的相似度，并找出最相似的 10 个用户
    private static void calUserNeighborhood() throws IOException, InterruptedException {
        // 使用 ExecutorService 创建线程池
        int numThreads = Runtime.getRuntime().availableProcessors(); // 获取可用的 CPU 核心数
        ExecutorService executorService = Executors.newFixedThreadPool(numThreads);

        // 使用 CountDownLatch 等待所有线程完成任务
        CountDownLatch latch = new CountDownLatch(userProducts.size());
        
        // 存储所有用户的计算结果
        List<Map.Entry<String, String[]>> resultList = Collections.synchronizedList(new ArrayList<>());

        // 为每个用户创建一个任务，并提交到线程池
        for (String userId : userProducts.keySet()) {
            executorService.submit(() -> {
                try {
                    // 最小堆存储与当前用户的相似度结果
                    PriorityQueue<Map.Entry<String, Double>> similarityQueue =
                            new PriorityQueue<>(10, Comparator.comparingDouble(Map.Entry::getValue));

                    // 计算与其他用户的相似度
                    for (String otherUserId : userProducts.keySet()) {
                        if (!userId.equals(otherUserId)) {
                            double similarity = calJaccard(userProducts.get(userId), userProducts.get(otherUserId));

                            // 如果堆的大小小于10，直接添加
                            if (similarityQueue.size() < 10) {
                                similarityQueue.add(new AbstractMap.SimpleEntry<>(otherUserId, similarity));
                            } else {
                                // 如果堆已经有10个元素，则与堆顶元素比较
                                if (similarity > similarityQueue.peek().getValue()) {
                                    similarityQueue.poll(); // 移除堆顶元素
                                    similarityQueue.add(new AbstractMap.SimpleEntry<>(otherUserId, similarity)); // 加入新的用户
                                }
                            }
                        }
                    }

                    // 构建结果字符串，用户和最相似的 10 个用户（按相似度从大到小排列）
                    StringBuilder result = new StringBuilder("userId: " + userId + ";\tUserNeighborhood: ");
                    String[] userNeighborhoods = new String[10];
                    List<Map.Entry<String, Double>> userNeighborhoodList = new ArrayList<>(similarityQueue);
                    userNeighborhoodList.sort((entry1, entry2) -> Double.compare(entry2.getValue(), entry1.getValue()));
                    
                    for (int i = 0; i < 10; ++i) {
                        userNeighborhoods[i] = userNeighborhoodList.get(i).getKey();
                    }
                    
                    // 将结果存入 resultList
                    resultList.add(new AbstractMap.SimpleEntry<>(userId, userNeighborhoods));

                }finally {
                    latch.countDown(); // 任务完成，减少计数
                }
            });
        }

        // 等待所有线程执行完毕
        latch.await();
        executorService.shutdown(); // 关闭线程池
        
        // 按 userID 从小到大排序
        resultList.sort((entry1, entry2) -> entry1.getKey().compareTo(entry2.getKey()));

        // 将排序后的结果写入到文件
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(outputFile))) {
            for (Map.Entry<String, String[]> entry : resultList) {
                StringBuilder result = new StringBuilder("userId: " + entry.getKey() + ";\tUserNeighborhood: ");
                String[] userNeighborhood = entry.getValue();

                // 将相似用户写入文件，以分号分隔
                for (int i = 0; i < 10; ++i) {
                    result.append(userNeighborhood[i]);
                    if (i != 9) {
                        result.append("; ");
                    }
                }

                bw.write(result.toString());
                bw.newLine();
            }
        }
    }

    public static void main(String[] args) {
        try {
            // 读取 review.txt 文件，构建用户-商品的映射
            readReviews();

            // 计算用户相似度并生成输出文件
            calUserNeighborhood();

            System.out.println("userNeighborhood.txt 已生成");

            // 检查 userNeighborhood.txt 前八行
            System.out.println("下面展示 userNeighborhood.txt 前八行数据：");
            try (BufferedReader br = new BufferedReader(new FileReader(outputFile))) {
                String line = "";
                for (int i = 0; i < 8; ++i) {
                    if ((line = br.readLine()) != null) {
                        System.out.println(line);
                    } else break;
                }
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}