package test4;

import java.io.IOException;

public class Test {
    public static void main(String[] args) {
        // 定义服务器端Java文件的路径
        String severFilePath = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test4\\Server.java";
        // 定义客户端Java文件的路径
        String clientFilePath = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src\\test4\\Client.java";
        // 定义类文件所在的目录
        String classDirectory = "C:\\Users\\12569\\Desktop\\code\\eclipse-workspace4\\Myproject\\src";
        
        // 启动新的终端并编译、运行服务器端
        startNewTerminalToCompileAndRun(severFilePath, classDirectory, "test4.Server");
        // 启动新的终端并编译、运行客户端
        startNewTerminalToCompileAndRun(clientFilePath, classDirectory, "test4.Client");
    }

    // 在新的终端中编译并运行指定的Java文件。
    private static void startNewTerminalToCompileAndRun(String filePath, String classDirectory, String className) {
        try {
            // 获取操作系统名称
            String os = System.getProperty("os.name").toLowerCase();
            // 如果操作系统是Windows
            if (os.contains("win")) {
                // 创建一个新的ProcessBuilder实例，用于启动新的命令行窗口
                ProcessBuilder pb = new ProcessBuilder(
                    "cmd", "/c", "start", "cmd.exe", "/K", 
                    // 编译Java文件并运行编译后的类文件，使用&&连接命令
                    "javac " + filePath + " && java -cp " + classDirectory + " " + className + " && pause"
                );
                // 继承父进程的输入输出流
                pb.inheritIO();
                // 启动进程
                pb.start();
            } else {
                // 如果操作系统不是Windows，则打印不支持的操作系统信息
                System.out.println("Unsupported operating system: " + os);
            }
        } catch (IOException e) {
            // 如果发生IO异常，则打印堆栈跟踪
            e.printStackTrace();
        }
    }
}