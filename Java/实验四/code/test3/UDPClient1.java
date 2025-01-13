package test3;

import java.net.*;
import java.util.Scanner;

public class UDPClient1 {
    private static String serverAddress = "localhost";  // 服务器地址
    private static int severPort = 9832;  // 服务器端口
    private static int clientPort = 9453;  // 客户端1的端口
    
    private static DatagramSocket clientSocket;  // 用于发送和接收UDP数据包的客户端套接字
    private static Scanner scanner;  // 用于获取用户输入
    private static byte[] receiveData = new byte[2048];  // 接收数据的缓冲区
    
    public static void main(String[] args) throws SocketException {
        // 创建DatagramSocket对象，绑定到客户端端口
        clientSocket = new DatagramSocket(clientPort);

        // 获取用户输入的消息
        scanner = new Scanner(System.in);

        // 输出客户端启动信息
        System.out.println("Client on port " + clientSocket.getLocalPort() + " established");
        
        // 启动发送消息的线程
        Thread workSendThread = new Thread(new WorkSend());
        // 启动接收响应消息的线程
        Thread workReceiveThread = new Thread(new WorkReceive());
        
        // 启动发送和接收线程
        workSendThread.start();
        workReceiveThread.start();
    }
    
    // 发送消息的线程
    private static class WorkSend implements Runnable {
        @Override
        public void run() {
            try {
                while (true) {
                    // 获取用户输入的消息
                    String message = scanner.nextLine();

                    // 如果用户输入"exit"，则退出
                    if (message.equalsIgnoreCase("exit")) {
                        System.out.println("EXIT!");
                        break;
                    }

                    // 将用户输入的消息转换成字节数组
                    byte[] sendData = message.getBytes();
                    // 获取服务器的地址对象
                    InetAddress serverAddressObj = InetAddress.getByName(serverAddress);
                    // 创建数据包，准备发送数据到服务器
                    DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddressObj, severPort);

                    // 发送数据包到服务器
                    clientSocket.send(sendPacket);
                }

                // 关闭客户端Socket和Scanner对象
                clientSocket.close();
                scanner.close();
            } catch (Exception e) {
                // 捕获异常并打印堆栈信息
                e.printStackTrace();
            }
        }
    }
    
    // 接收消息的线程
    private static class WorkReceive implements Runnable {
        @Override
        public void run() {
            try {
                while (true) {
                    // 定义接收响应的数据包
                    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);

                    // 接收服务器的响应数据包
                    clientSocket.receive(receivePacket);

                    // 从数据包中获取响应的消息内容
                    String responseMessage = new String(receivePacket.getData(), 0, receivePacket.getLength());
                    // 输出收到的响应消息
                    System.out.println("Receive Response form another client: " + responseMessage);
                }
            } catch (Exception e) {
                // 捕获异常并打印堆栈信息
                e.printStackTrace();
            }
        }
    }
}
