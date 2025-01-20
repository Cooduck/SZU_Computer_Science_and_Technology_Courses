package RSA;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.security.MessageDigest;
import java.io.*;
import java.util.Arrays;

public class RSA {
	// 简校滨 2022150176
	private BigInteger n, d, e; // 模数 n，私钥 d，公钥 e
	private int bitlen = 2048; // 密钥长度，通常为2048位，以确保安全性
	private int nlen; // n 的位数，用于后续读取二进制明文的的位数
    
	// 构造函数：生成公钥和私钥 
	public RSA() {
	    SecureRandom random = new SecureRandom(); // 初始化安全随机数生成器
	    BigInteger p = BigInteger.probablePrime(bitlen / 2, random); // 生成大素数 p
	    BigInteger q = BigInteger.probablePrime(bitlen / 2, random); // 生成大素数 q
	    n = p.multiply(q); // 计算模数 n = p * q
	    nlen = n.bitLength(); // 计算 n 的位数
	    // 计算欧拉函数 φ(n)
	    BigInteger phi = (p.subtract(BigInteger.ONE)).multiply(q.subtract(BigInteger.ONE));
	    e = BigInteger.probablePrime(bitlen, random); // 选择公钥 e，需为大素数
	    // 确保 e 与 φ(n) 互质
	    while (phi.gcd(e).compareTo(BigInteger.ONE) != 0) {
	        e = BigInteger.probablePrime(bitlen, random); // 重新选择 e
	    }
	    d = e.modInverse(phi); // 计算私钥 d，使得 e * d ≡ 1 (mod φ(n))
	}
    
	// 加密函数：使用公钥(n, e) 
	public BigInteger encrypt(BigInteger message) {
	    return message.modPow(e, n); // 计算 ciphertext = message^e mod n
	}

	// 解密函数：使用私钥(n, d) 
	public BigInteger decrypt(BigInteger encrypted) {
	    return encrypted.modPow(d, n); // 计算 plaintext = ciphertext^d mod n
	}

    
	// 将文件内容加密并保存到新文件 
	public void encryptFile(String inputFilePath, String outputFilePath) throws IOException {
	    try (FileInputStream fis = new FileInputStream(inputFilePath);
	         BufferedWriter writer = new BufferedWriter(new FileWriter(outputFilePath))) {

	    	 // 每次读取 (nlen - 1) 位，即 (nlen - 1) / 8 字节，确保 M 小于 n
	        byte[] buffer = new byte[(nlen - 1) / 8];
	        int bytesRead;

	        while ((bytesRead = fis.read(buffer)) != -1) {
	            BigInteger message = new BigInteger(1, buffer); // 将读取的字节转换为 BigInteger
	            BigInteger encryptedMessage = encrypt(message); // 加密
	            
	            // 写入加密后的数据到新文件，以换行符分隔
	            writer.write(encryptedMessage.toString());
	            writer.newLine(); // 写入新的一行
	            
	            Arrays.fill(buffer, (byte) 0); // 清空 buffer
	        }
	    }
	}

	// 将加密文件内容解密并保存到新文件 
	public void decryptFile(String inputFilePath, String outputFilePath) throws IOException {
	    try (BufferedReader reader = new BufferedReader(new FileReader(inputFilePath));
	         FileOutputStream fos = new FileOutputStream(outputFilePath)) {
	        
	        String line;
	        while ((line = reader.readLine()) != null) { // 逐行读取
	            BigInteger encryptedMessage = new BigInteger(line); // 将读取的行转换为 BigInteger
	            BigInteger decryptMessage = decrypt(encryptedMessage); // 解密
	            
	            // 将解密后的数据写入文件
	            fos.write(decryptMessage.toByteArray());
	            Arrays.fill(decryptMessage.toByteArray(), (byte) 0); // 清空解密后的字节
	        }
	    }
	}
    
	// 签名函数：使用私钥签名文件内容 
	public BigInteger signFile(String filePath) throws Exception {
	    // 读取文件内容
	    StringBuilder fileContent = new StringBuilder(); // 创建一个 StringBuilder 用于拼接文件内容
	    try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) { // 使用 BufferedReader 读取文件
	        String line;
	        while ((line = reader.readLine()) != null) { // 按行读取文件
	            fileContent.append(line).append("\n"); // 将每一行内容拼接到 StringBuilder 中，并添加换行符
	        }
	    }

	    // 计算文件内容的摘要值并加密
	    MessageDigest md = MessageDigest.getInstance("SHA-256"); // 创建 SHA-256 的 MessageDigest 实例
	    byte[] hash = md.digest(fileContent.toString().getBytes("UTF-8")); // 将文件内容转换为字节数组并计算其摘要值
	    BigInteger hashInt = new BigInteger(1, hash); // 将摘要值转换为 BigInteger，1表示无符号
	    return encrypt(hashInt); // 使用私钥对摘要值进行加密，返回加密后的数字签名
	}


	// 验证签名函数：使用公钥验证文件签名 
	public boolean verifyFile(String filePath, BigInteger signature) throws Exception {
	    // 读取文件内容
	    StringBuilder fileContent = new StringBuilder(); // 创建一个 StringBuilder 用于拼接文件内容
	    try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) { // 使用 BufferedReader 读取文件
	        String line;
	        while ((line = reader.readLine()) != null) { // 按行读取文件
	            fileContent.append(line).append("\n"); // 将每一行内容拼接到 StringBuilder 中，并添加换行符
	        }
	    }

	    // 计算文件内容的哈希值
	    MessageDigest md = MessageDigest.getInstance("SHA-256"); // 创建 SHA-256 的 MessageDigest 实例
	    byte[] hash = md.digest(fileContent.toString().getBytes("UTF-8")); // 将文件内容转换为字节数组并计算其摘要值
	    BigInteger hashInt = new BigInteger(1, hash); // 将摘要值转换为 BigInteger，1表示无符号

	    // 解密签名得到哈希值
	    BigInteger decryptedHash = decrypt(signature); // 使用公钥解密签名以获取摘要值
	    return decryptedHash.equals(hashInt); // 比较解密后的摘要值与原摘要值
	}


    // 定义主函数，用于测试加密和解密、数字签名 
    public static void main(String[] args) {
    	// RSA 加密与解密演示 
    	RSA rsa = new RSA(); // 创建 RSA 实例以生成公钥和私钥
    	System.out.println("加密和解密：");
    	System.out.println("模数 n：" + rsa.n); // 打印模数 n
    	System.out.println("公匙 e：" + rsa.e); // 打印公钥 e
    	System.out.println("私钥 d：" + rsa.d); // 打印私钥 d

    	// 定义文件路径
    	String plain = "C:\\Users\\12569\\Desktop\\code\\ComputerSafe\\task2\\task2"
    			+ "\\src\\RSA\\plain.txt";  // 输入的明文文件
    	String encryptedFile = "C:\\Users\\12569\\Desktop\\code\\ComputerSafe\\task2"
    			+ "\\task2\\src\\RSA\\encryptedFile.txt";  // 输出的加密文件
    	String decryptFile = "C:\\Users\\12569\\Desktop\\code\\ComputerSafe\\task2\\task2"
    			+ "\\src\\RSA\\decryptFile.txt";  // 输出的解密文件

    	// 加密和解密文件 
    	try {
    	    rsa.encryptFile(plain, encryptedFile); // 调用加密方法，将明文文件加密并保存为新文件
    	    System.out.println("明文已加密并保存到 " + encryptedFile); // 打印加密成功消息
    	    rsa.decryptFile(encryptedFile, decryptFile); // 调用解密方法，将密文文件解密并保存为新文件
    	    System.out.println("密文已解密并保存到 " + decryptFile); // 打印解密成功消息
    	} catch (IOException e) {
    	    e.printStackTrace(); // 捕获并打印 IO 异常信息
    	}

        
        System.out.println();
        
        // 数字签名文件内容 
        String message = "C:\\Users\\12569\\Desktop\\code\\ComputerSafe\\task2\\task2"
                         + "\\src\\RSA\\message.txt"; // 输入的文件路径
        try {
            BigInteger signature = rsa.signFile(message); // 对文件内容进行签名，生成数字签名
            System.out.println("文件的数字签名：" + signature); // 输出生成的数字签名

            // 验证签名
            boolean isVerified = rsa.verifyFile(message, signature); // 验证签名的有效性
            System.out.println("签名验证结果：" + isVerified); // 输出签名验证结果
        } catch (Exception e) {
            e.printStackTrace(); // 捕获并打印异常信息
        }


    }
}
