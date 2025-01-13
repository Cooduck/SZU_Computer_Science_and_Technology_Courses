package test6;

//接口Human，定义了sayHello方法
public interface Human2 {
	 double sayHello();
}

//Chinese类实现Human接口
class Chinese2 implements Human2 {
	 private String name; // 姓名成员变量
	 
	 // 构造方法，初始化姓名
	 public Chinese2(String name) {
	     this.name = name;
	 }
	 
	 // 实现接口中的sayHello方法
	 @Override
	 public double sayHello() {
	     System.out.println("你好, 我是" + name + "!"); // 输出中文问候
	     return 0.0; // 返回0.0
	 }
}

//Japanese类实现Human接口
class Japanese2 implements Human2 {
	 private String name; // 姓名成员变量
	 
	 // 构造方法，初始化姓名
	 public Japanese2(String name) {
	     this.name = name;
	 }
	 
	 // 实现接口中的sayHello方法
	 @Override
	 public double sayHello() {
	     System.out.println("こんにちは, 私は" + name + "!"); // 输出日文问候
	     return 0.0; // 返回0.0
	 }
}

//English类实现Human接口
class English2 implements Human2 {
	 private String name; // 姓名成员变量
	 
	 // 构造方法，初始化姓名
	 public English2(String name) {
	     this.name = name;
	 }
	 
	 // 实现接口中的sayHello方法
	 @Override
	 public double sayHello() {
	     System.out.println("Hello, I am " + name + "!"); // 输出英文问候
	     return 0.0; // 返回0.0
	 }
}
