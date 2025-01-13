package test4;

import java.io.*;
import java.net.*;
import java.util.*;

class Question {
    private String questionText;     // 题目文本
    private String correctAnswers;   // 正确答案
    private String[] options;        // 题目选项
    private int type;                // 题目类型: 0-单选，1-多选，2-判断

    // 构造函数，初始化题目属性
    public Question(int type, String questionText, String correctAnswers, String... options) {
        this.type = type;
        this.questionText = questionText;
        this.correctAnswers = correctAnswers;
        this.options = options;
    }
    
    // 获取题目类型
    public int getType() {
        return type;
    }

    // 获取题目文本
    public String getQuestionText() {
        return questionText;
    }

    // 获取正确答案
    public String getCorrectAnswers() {
        return correctAnswers;
    }

    // 获取选项数组
    public String[] getOptions() {
        return options;
    }
}

public class Server {
    private static ServerSocket serverSocket; // 服务器端口监听
    private static Socket clientSocket; // 客户端连接
    private static BufferedReader in; // 从客户端读取输入
    private static PrintWriter out; // 向客户端发送输出

    // 题库：单选题、多选题、判断题
    private static final List<Question> singleChoiceQuestions = new ArrayList<>();
    private static final List<Question> multiChoiceQuestions = new ArrayList<>();
    private static final List<Question> trueFalseQuestions = new ArrayList<>();
    
    // 题目计数和答案统计
    private static int singleChoiceCount = 0;    // 单选题已出题数量
    private static int multiChoiceCount = 0;     // 多选题已出题数量
    private static int trueFalseCount = 0;       // 判断题已出题数量
    private static int correctSingleChoice = 0;  // 单选题正确数
    private static int correctMultiChoice = 0;   // 多选题正确数
    private static int correctTrueFalse = 0;     // 判断题正确数
    private static int totScore = 0;             // 总得分
    private static int[] finishFlag = {0, 0, 0}; // 记录某类题目是否出题已达上限，1为达到上限
    
    private static int currentQuestionType; // 当前出题类型
    private static String[] questionTypeName = {"单选题", "多选题", "判断题"};
    
    private static int currentQuestionIdx = 1;	// 当前已出题的题数

    public static void main(String[] args) {
        while(true) {
            try {
                // 创建服务器端口监听
                serverSocket = new ServerSocket(12345);
            } catch(IOException e1) {
                System.out.println("正在监听"); // 如果端口已经被监听，则打印
            }

            try {
                // 等待客户端连接
                clientSocket = serverSocket.accept();
                System.out.println("用户的地址:" + clientSocket.getInetAddress());
            } catch (IOException e) {
                System.out.println("正在等待用户");
            }

            if(clientSocket != null) {
                // 客户端连接后启动一个线程处理客户端请求
                Thread workThread = new Thread(Server::work);
                workThread.start();
            } else {
                continue; // 如果没有客户端连接，继续监听
            }
        }
    }

    private static void work() {
        // 初始化题库
        initQuestions();

        try {
            // 获取输入输出流
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            out = new PrintWriter(clientSocket.getOutputStream(), true);

            while (true) {
                // 等待客户端发送 "ready" 信号
                String readySignal = in.readLine();
                if (readySignal != null && readySignal.equals("exam finish")) {
                    // 客户端完成考试，发送总分数并结束
                    out.println(totScore + "");
                    break;
                } else if (readySignal != null && readySignal.equals("ready")) {
                    // 客户端准备好开始考试，随机选择一道题
                    Question question = getRandomQuestion();
                    sendQuestionToClient(question);
                    System.out.println("发送问题: " + question.getQuestionText());

                    // 接收客户端的答案
                    String answer = in.readLine();
                    System.out.println("得到回答: " + answer);

                    // 判断答案是否正确
                    String result = evaluateAnswer(question, answer);
                    out.println(result);  // 发送答案判断结果到客户端
                    out.println(question.getCorrectAnswers()); // 发送正确答案
                    System.out.println("判断结果: " + result);

                    // 更新答题状态
                    updateState(result);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 初始化题库，添加题目到题库中
    private static void initQuestions() {
        // 单选题
        singleChoiceQuestions.add(new Question(0, "3+3等于多少？", "B", "A. 5", "B. 6", "C. 9", "D. 10"));
        singleChoiceQuestions.add(new Question(0, "5*5等于多少？", "A", "A. 25", "B. 30", "C. 35", "D. 40"));
        singleChoiceQuestions.add(new Question(0, "Java中哪个不是控制流程语句？", "D", "A. if", "B. for", "C. switch", "D. try"));
        singleChoiceQuestions.add(new Question(0, "哪个是Java中的集合类？", "B", "A. String", "B. ArrayList", "C. int", "D. float"));
        singleChoiceQuestions.add(new Question(0, "谁发现了地心引力？", "C", "A. 爱因斯坦", "B. 麦克雷", "C. 牛顿", "D. 毕加索"));
        
        // 多选题
        multiChoiceQuestions.add(new Question(1, "Java中哪些是访问修饰符？", "A,C", "A. public", "B. static", "C. private", "D. final"));
        multiChoiceQuestions.add(new Question(1, "哪些是Java中的方法重载的条件？", "A,B,C,D", "A. 方法名相同", "B. 参数列表不同", "C. 返回值可以不同", "D. 方法的修饰符可以不同"));
        multiChoiceQuestions.add(new Question(1, "哪些是Java中的异常类型？", "A,B", "A. RuntimeException", "B. IOException", "C. String", "D. Object"));
        multiChoiceQuestions.add(new Question(1, "哪些是Java中的接口？", "A,C,D", "A. Cloneable", "B. String", "C. Runnable", "D. Serializable"));
        multiChoiceQuestions.add(new Question(1, "深圳大学有哪些校区？", "A,C", "A. 粤海校区", "B. 后海校区", "C. 丽湖校区", "D. 蛇口校区"));
        
        // 判断题
        trueFalseQuestions.add(new Question(2, "Java中的main方法必须返回void。", "A", "A. 正确", "B. 错误"));
        trueFalseQuestions.add(new Question(2, "Java支持多继承。", "B", "A. 正确", "B. 错误"));
        trueFalseQuestions.add(new Question(2, "Java中的String是可变的。", "B", "A. 正确", "B. 错误"));
        trueFalseQuestions.add(new Question(2, "Java中的final关键字可以修饰类、方法和变量。", "A", "A. 正确", "B. 错误"));
        trueFalseQuestions.add(new Question(2, "深圳大学是985。", "B", "A. 正确", "B. 错误"));
    }

    // 随机选择一题
    private static Question getRandomQuestion() {
        Random rand = new Random();
        // 随机选择题目类型
        currentQuestionType = rand.nextInt(3);
        while(finishFlag[currentQuestionType] == 1) {
            currentQuestionType = rand.nextInt(3); // 如果该类型题目已达上限，重新选择
        }

        if(currentQuestionType == 0) {
            // 单选题
            singleChoiceCount++;
            if(singleChoiceCount >= 5) finishFlag[0] = 1; // 达到题目上限
            return singleChoiceQuestions.get(rand.nextInt(singleChoiceQuestions.size()));
        }
        else if(currentQuestionType == 1) {
            // 多选题
            multiChoiceCount++;
            if(multiChoiceCount >= 5) finishFlag[1] = 1; // 达到题目上限
            return multiChoiceQuestions.get(rand.nextInt(multiChoiceQuestions.size()));
        }
        else {
            // 判断题
            trueFalseCount++;
            if(trueFalseCount >= 5) finishFlag[2] = 1; // 达到题目上限
            return trueFalseQuestions.get(rand.nextInt(trueFalseQuestions.size()));
        }
    }

    // 发送问题到客户端
    private static void sendQuestionToClient(Question question) {
    	StringBuffer questionBuffer = new StringBuffer();
    	questionBuffer.append(currentQuestionIdx++ + ".(" + questionTypeName[question.getType()] + ") " + question.getQuestionText());
    	out.println(questionBuffer.toString());
    	String[] options = question.getOptions();
        for (String option : options) {
            out.println(option);
        }
        out.println("finish");
    }

    // 判断答案是否正确
    private static String evaluateAnswer(Question question, String answer) {
        return answer.equals(question.getCorrectAnswers()) ? "正确" : "错误";
    }

    // 更新答题状态
    private static void updateState(String result) {
        if(result.equals("正确")) {
            if(currentQuestionType == 0) {
            	correctSingleChoice++;
            	totScore++;
            }
            else if(currentQuestionType == 1) {
            	correctMultiChoice++;
            	totScore += 2;
            }
            else if(currentQuestionType == 2) {
            	correctTrueFalse++;
            	totScore++;
            }
        }
        
        String stats = String.format("作答情况： 单选题：%d/%d  多选题：%d/%d  判断题：%d/%d  成绩：%d",
                correctSingleChoice, singleChoiceCount,
                correctMultiChoice, multiChoiceCount,
                correctTrueFalse, trueFalseCount,
                totScore);
        out.println(stats);
    }
}
