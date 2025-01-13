package test3;

import java.io.IOException;
import java.net.*;

public class UDPServer {
    // 定义接收和发送的数据包
    private static byte[] receiveData = new byte[2048];  // 用于接收客户端数据的缓冲区
    private static byte[] sendData;  // 用于发送数据到客户端的缓冲区
    
    private static DatagramSocket serverSocket;  // 用于接收和发送数据包的DatagramSocket对象
    
    private static int severPort = 9832;  // 服务器端口
    private static int client1Port = 9453;  // 客户端1端口
    private static int client2Port = 8643;  // 客户端2端口
    
    public static void main(String[] args) throws SocketException {
        // 创建DatagramSocket对象，并绑定到服务器端口
        serverSocket = new DatagramSocket(severPort);
        System.out.println("Server is listening on port " + severPort);
        
        // 启动一个线程来处理接收到的数据
        Thread workThread = new Thread(new work());
        workThread.start();
    }
    
    // 内部类work实现Runnable接口，用于接收并转发客户端消息
    private static class work implements Runnable {
        @Override
        public void run() {
            try {
                // 无限循环，持续接收客户端数据并转发
                while(true) {
                    // 创建一个DatagramPacket对象来接收数据包
                    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                    // 接收数据包
                    serverSocket.receive(receivePacket);

                    // 从接收到的数据包中获取客户端的地址和端口
                    InetAddress clientAddress = receivePacket.getAddress();  // 客户端IP地址
                    int clientPort = receivePacket.getPort();  // 客户端端口号

                    // 获取客户端发送的消息，并输出
                    String message = new String(receivePacket.getData(), 0, receivePacket.getLength());
                    System.out.println("Server received message from client(IP:" + clientAddress.getHostAddress() + "/Port:" + clientPort + "): " + message);

                    // 根据客户端的端口决定消息应该转发到哪个客户端
                    int sendClientPort = 0;
                    if(clientPort == client1Port) {
                        sendClientPort = client2Port;  // 如果消息来自客户端1，则转发到客户端2
                    } else {
                        sendClientPort = client1Port;  // 如果消息来自客户端2，则转发到客户端1
                    }

                    // 将接收到的消息封装成发送数据包
                    sendData = message.getBytes();
                    DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientAddress, sendClientPort);

                    // 发送响应数据包
                    serverSocket.send(sendPacket);
                    System.out.println("Server forwarded message to client(IP:" + clientAddress.getHostAddress() + "/Port:" + sendClientPort + ")");
                }
            }
            catch (Exception e) {
                // 捕获异常并打印堆栈信息
                e.printStackTrace();
            }
        }
    }
}
