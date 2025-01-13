package test3;

import java.io.IOException;

public class Test {
    public static void main(String[] args) {
        // 服务器和客户端Java源文件的路径
        String severFilePath = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test3\\UDPServer.java";
        String client1FilePath = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test3\\UDPClient1.java";
        String client2FilePath = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test3\\UDPClient2.java";
        
        // Java类文件所在的目录路径
        String classDirectory = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src";
        
        // 启动新终端窗口，编译并运行 UDPServer、UDPClient1 和 UDPClient2
        startNewTerminalToCompileAndRun(severFilePath, classDirectory, "test3.UDPServer");
        startNewTerminalToCompileAndRun(client1FilePath, classDirectory, "test3.UDPClient1");
        startNewTerminalToCompileAndRun(client2FilePath, classDirectory, "test3.UDPClient2");
    }


    // 启动一个新的终端窗口来编译并运行指定的Java源文件
    private static void startNewTerminalToCompileAndRun(String filePath, String classDirectory, String className) {
        try {
            // 获取操作系统的名称，判断是否为Windows
            String os = System.getProperty("os.name").toLowerCase();
            if (os.contains("win")) {
                // 如果是Windows系统，使用cmd.exe启动新的命令行窗口执行编译和运行的命令
                ProcessBuilder pb = new ProcessBuilder(
                    "cmd", "/c", "start", "cmd.exe", "/K", 
                    // 编译源文件并运行相应的Java类，同时保持命令行窗口打开
                    "javac " + filePath + " && java -cp " + classDirectory + " " + className + " && pause"
                );
                pb.inheritIO();  // 继承父进程的输入输出流
                pb.start();  // 启动进程
            } 
            else {
                // 如果操作系统不是Windows，打印不支持的操作系统类型
                System.out.println("Unsupported operating system: " + os);
            }
        } catch (IOException e) {
            // 捕获并打印可能发生的异常
            e.printStackTrace();
        }
    }
}
