package test1;

import java.io.*;
import java.util.*;

public class GetReview {
    // Review类，用于存储每个评论的用户ID，产品ID和评论摘要
    private static class Review {
        private String userID, productID, summary;

        // 构造函数，初始化Review对象的三个字段
        public Review(String userID, String productID, String summary) {
            this.userID = userID;
            this.productID = productID;
            this.summary = summary;
        }

        // 获取用户ID
        public String getUserID() { return userID; }

        // 获取产品ID
        public String getProductID() { return productID; }

        // 获取评论摘要
        public String getSummary() { return summary; }
    }

    public static void main(String[] args) {
        // 输入文件和输出文件路径
        String inputFile = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test1\\Watches.txt";
        String outputFile = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test1\\review.txt";

        // 存储每个评论的三元组(userID, productID, summary)的列表
        List<Review> reviews = new ArrayList<>();

        try (BufferedReader br = new BufferedReader(new FileReader(inputFile))) {
            String line;
            String userId = "";  // 用于存储当前读取到的用户ID
            String productId = "";  // 用于存储当前读取到的产品ID
            String summary = "";  // 用于存储当前读取到的评论摘要

            // 持续读取每一行数据，直到文件结束
            while ((line = br.readLine()) != null) {
                // 解析产品ID
                if (line.startsWith("product/productId:")) {
                    productId = line.split(":")[1].trim();  // 获取冒号后的ID并去除前后空格
                } 
                // 解析用户ID
                else if (line.startsWith("review/userId:")) {
                    userId = line.split(":")[1].trim();  // 获取冒号后的ID并去除前后空格
                } 
                // 解析评论摘要
                else if (line.startsWith("review/summary:")) {
                    summary = line.split(":")[1].trim();  // 获取冒号后的摘要并去除前后空格
                    // 一旦读取到完整的评论信息 (userId, productId, summary)，就创建一个Review对象并加入到列表中
                    reviews.add(new Review(userId, productId, summary));
                }
            }
            System.out.println("Watches.txt 的数据已全部读取");

            // 按照 userId 和 productId 对评论列表进行排序
            reviews.sort((a, b) -> {
                // 首先按用户ID进行排序
                int userIdComparison = a.getUserID().compareTo(b.getUserID());
                if (userIdComparison != 0) {
                    return userIdComparison;  // 如果用户ID不同，则按用户ID排序
                } else {
                    // 如果用户ID相同，则按产品ID进行排序
                    return a.getProductID().compareTo(b.getProductID());
                }
            });

            // 将排序后的评论数据写入输出文件
            try (BufferedWriter bw = new BufferedWriter(new FileWriter(outputFile))) {
                for (Review review : reviews) {
                    // 写入每一条评论的数据：userID, productID, summary
                    bw.write("userID: " + review.getUserID() + ";\t" + 
                             "productID: " + review.getProductID() + ";\t" + 
                             "summary: " + review.getSummary());
                    bw.newLine();  // 每条评论结束后换行
                }
                System.out.println("review.txt 已生成");
            }

            // 检查并展示生成的 review.txt 文件的前八行数据
            System.out.println("下面展示 review.txt 前八行数据：");
            try (BufferedReader br2 = new BufferedReader(new FileReader(outputFile))) {
                for (int i = 0; i < 8; ++i) {
                    // 读取并打印前八行，如果文件不够八行，则只输出剩余行
                    if ((line = br2.readLine()) != null) {
                        System.out.println(line);
                    } else {
                        break;
                    }
                }
            }

        } catch (IOException e) {
            // 捕获并打印异常
            e.printStackTrace();
        }
    }
}
