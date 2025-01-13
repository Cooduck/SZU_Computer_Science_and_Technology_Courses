package test3;

public class Queue {
    private float[] elements;  // 用于存储float型数据的数组
    private int size;          // 当前队列中元素的数量
    private int capacity;      // 队列的容量
    private int front;         // 队列的头部索引
    private int rear;          // 队列的尾部索引

    // 构造方法，初始化队列容量为12
    public Queue() {
        capacity = 12;                       // 设置队列的容量
        elements = new float[capacity];      // 初始化存储数组
        size = 0;                            // 当前元素数量为0
        front = 0;                           // 头部索引初始化
        rear = -1;                           // 尾部索引初始化
    }

    // 往队列中添加一个float型数据
    public void enqueue(float v) {
        if (size >= capacity) {              // 检查队列是否已满
            System.out.println("The queue is full and no new elements can be added!");
        } else {
            rear = (rear + 1) % capacity;    // 更新尾部索引
            elements[rear] = v;               // 添加新元素
            size++;                           // 增加元素数量
        }
    }

    // 从队列中删除并返回一个float型数据
    public float dequeue() {
        if (size == 0) {                      // 检查队列是否为空
            System.out.println("The queue is empty and the element cannot be removed!");
            return -1;                        // 返回-1表示错误
        } else {
            float value = elements[front];    // 获取头部元素
            front = (front + 1) % capacity;   // 更新头部索引
            size--;                           // 减少元素数量
            return value;                     // 返回移除的元素
        }
    }

    // 返回队列中的元素个数
    public int getSize() {
        return size;                          // 返回当前队列大小
    }
    
    public static void main(String[] args) {
        Queue q = new Queue();               // 创建Queue对象
        
        q.enqueue(1.1f);                     // 向队列添加元素
        q.enqueue(2.2f);
        q.enqueue(3.3f);
        System.out.println("Queue size: " + q.getSize()); // 输出当前队列大小

        // 从队列中取出元素并输出
        System.out.println("Retrieving Elements: " + q.dequeue());
        System.out.println("Retrieving Elements: " + q.dequeue());
        System.out.println("Queue size: " + q.getSize());
        System.out.println("Retrieving Elements: " + q.dequeue());
        System.out.println("Retrieving Elements: " + q.dequeue());
    }
}
