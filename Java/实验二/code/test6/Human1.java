package test6;

//定义抽象类Human1，表示人类的基本特征
public abstract class Human1 {
	protected String name; // 保护属性，存储人类的姓名
	 
	// 构造方法，初始化name属性
	public Human1(String name) {
	    this.name = name; // 将传入的姓名赋值给name属性
	}
	 
	// 抽象方法sayHello，子类必须实现该方法
	public abstract double sayHello();
}


//Chinese1类，继承自Human1，表示中国人
class Chinese1 extends Human1 {
	 // 构造方法，调用父类的构造方法初始化姓名
	 public Chinese1(String name) {
	     super(name);
	 }
	 
	 // 实现sayHello方法，输出中文问候
	 @Override
	 public double sayHello() {
	     System.out.println("你好, 我是" + name + "!");
	     return 0.0; // 返回0.0，符合抽象方法的返回类型
	 }
}

//Japanese1类，继承自Human1，表示日本人
class Japanese1 extends Human1 {
	 // 构造方法，调用父类的构造方法初始化姓名
	 public Japanese1(String name) {
	     super(name);
	 }
	 
	 // 实现sayHello方法，输出日文问候
	 @Override
	 public double sayHello() {
	     System.out.println("こんにちは, 私は" + name + "!");
	     return 0.0; // 返回0.0，符合抽象方法的返回类型
	 }
}

//English1类，继承自Human1，表示英国人
class English1 extends Human1 {
	 // 构造方法，调用父类的构造方法初始化姓名
	 public English1(String name) {
	     super(name);
	 }
	 
	 // 实现sayHello方法，输出英文问候
	 @Override
	 public double sayHello() {
	     System.out.println("Hello, I am " + name + "!");
	     return 0.0; // 返回0.0，符合抽象方法的返回类型
	 }
}

