package test2;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.util.List;
import java.util.Timer;
import java.util.concurrent.TimeUnit;

class Question {
    private String questionText;	// 题目内容
    private String correctAnswers;	// 题目答案
    private String[] options;		// 题目选项
    private int type;	// 0为单选，1为多选，2为判断
    
    public Question(int type, String questionText, String correctAnswers, String... options) {
    	this.type = type;
    	this.questionText = questionText;
        this.correctAnswers = correctAnswers;
        this.options = options;
    }
    
    public int getType() {
        return type;
    }
    
    public String getQuestionText() {
        return questionText;
    }
    
    public String getCorrectAnswers() {
        return correctAnswers;
    }
    
    public String[] getOptions() {
        return options;
    }
}

public class ExamApp {
    
    // 题库
    private static final List<Question> singleChoiceQuestions = new ArrayList<>();
    private static final List<Question> multiChoiceQuestions = new ArrayList<>();
    private static final List<Question> trueFalseQuestions = new ArrayList<>();
    
    // 题目计数和答案统计
    private static int singleChoiceCount = 0;	// 单选题题数
    private static int multiChoiceCount = 0;	// 多选题题数
    private static int trueFalseCount = 0;		// 判断题题数
    private static int correctSingleChoice = 0;	// 单选题正确题数
    private static int correctMultiChoice = 0;	// 多选题正确题数
    private static int correctTrueFalse = 0;	// 判断题正确题数
    private static int totScore = 0;	// 总得分
    private static int[] finishFlag = {0, 0, 0};	// 如果某类型题目出题达到上限，则置1
    
    // 当前问题和用户答案
    private static Question currentQuestion;
    private static int currentQuestiontype;
    
    private static String[] questionTypeName = {"单选题", "多选题", "判断题"};
    
    // 图形化界面变量
    private static JFrame frame;
    private static JPanel questionPanel;
    private static JLabel questionLabel;
    private static JLabel statsLabel;
    private static JLabel timerLabel;
    private static JPanel answerPanel;
    private static JButton submitButton;
    private static JLabel resultLabel;
    
    // 计时器实现所需变量
    private static long startTime;
    private static long elapsedTime;
    private static boolean isRunning = true;
    
    // 字体
    private static Font font = new Font("Label.font", Font.BOLD | Font.PLAIN, 16); // 16是字体大小
    
    // 添加一个标志，表示是否已经点击了提交按钮
    private static boolean isSubmitClicked = false;
    
    public static void main(String[] args) {
        // 初始化题库
        initQuestions();
        
        // 创建窗口
        frame = new JFrame("Java机考");
        frame.setSize(500, 500);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        // 创建主面板
        questionPanel = new JPanel();
        questionPanel.setLayout(new BorderLayout());
        
        // 创建状态标签
        statsLabel = new JLabel("作答情况：单选题：0/0 多选题：0/0  判断题：0/0  成绩：0", SwingConstants.LEFT);
        statsLabel.setFont(font);
        
        // 创建问题标签
        questionLabel = new JLabel("题目会在此显示", SwingConstants.LEFT);
        questionLabel.setFont(font);
        
        // 创建时间标签
        timerLabel = new JLabel("用时: 00:00", SwingConstants.LEFT);
        timerLabel.setFont(font);
        
        // 创建一个新的面板，用于放置状态标签、问题标签、时间标签
        JPanel topPanel = new JPanel();
        topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.Y_AXIS));  // 设置为垂直布局
        topPanel.add(timerLabel);  // 添加时间标签
        topPanel.add(statsLabel);  // 添加状态标签
        topPanel.add(questionLabel);  // 添加问题标签
        questionPanel.add(topPanel, BorderLayout.NORTH);
        
        // 创建答题选项面板
        answerPanel = new JPanel();
        answerPanel.setLayout(new BoxLayout(answerPanel, BoxLayout.Y_AXIS));
        questionPanel.add(answerPanel, BorderLayout.CENTER);
        
        resultLabel = new JLabel("", SwingConstants.LEFT);
        resultLabel.setFont(font);
        
        // 创建提交按钮
        submitButton = new JButton("提交");
        submitButton.addActionListener(e -> submitAnswer());
        questionPanel.add(submitButton, BorderLayout.SOUTH);
        
        frame.add(questionPanel);
        frame.setVisible(true);
        
        // 启动工作线程和计时线程
        Thread workThread = new Thread(ExamApp::work);
        Thread timeThread = new Thread(ExamApp::Timer);
        workThread.start();
        timeThread.start();
    }
    
    // 初始化题库
    private static void initQuestions() {
    	singleChoiceQuestions.add(new Question(0, "3+3等于多少？", "B", "A. 5", "B. 6", "C. 9", "D. 10"));
    	singleChoiceQuestions.add(new Question(0, "5*5等于多少？", "A", "A. 25", "B. 30", "C. 35", "D. 40"));
    	singleChoiceQuestions.add(new Question(0, "Java中哪个不是控制流程语句？", "D", "A. if", "B. for", "C. switch", "D. try"));
    	singleChoiceQuestions.add(new Question(0, "哪个是Java中的集合类？", "B", "A. String", "B. ArrayList", "C. int", "D. float"));
    	singleChoiceQuestions.add(new Question(0, "谁发现了地心引力？", "C", "A. 爱因斯坦", "B. 麦克雷", "C. 牛顿", "D. 毕加索"));
    	multiChoiceQuestions.add(new Question(1, "Java中哪些是访问修饰符？", "A,C", "A. public", "B. static", "C. private", "D. final"));
    	multiChoiceQuestions.add(new Question(1, "哪些是Java中的方法重载的条件？", "A,B,C,D", "A. 方法名相同", "B. 参数列表不同", "C. 返回值可以不同", "D. 方法的修饰符可以不同"));
    	multiChoiceQuestions.add(new Question(1, "哪些是Java中的异常类型？", "A,B", "A. RuntimeException", "B. IOException", "C. String", "D. Object"));
    	multiChoiceQuestions.add(new Question(1, "哪些是Java中的接口？", "A,C,D", "A. Cloneable", "B. String", "C. Runnable", "D. Serializable"));
    	multiChoiceQuestions.add(new Question(1, "深圳大学有哪些校区？", "A,C", "A. 粤海校区", "B. 后海校区", "C. 丽湖校区", "D. 蛇口校区"));
    	trueFalseQuestions.add(new Question(2, "Java中的main方法必须返回void。", "A", "A. 正确", "B. 错误"));
    	trueFalseQuestions.add(new Question(2, "Java支持多继承。", "B", "A. 正确", "B. 错误"));
    	trueFalseQuestions.add(new Question(2, "Java中的String是可变的。", "B", "A. 正确", "B. 错误"));
    	trueFalseQuestions.add(new Question(2, "Java中的final关键字可以修饰类、方法和变量。", "A", "A. 正确", "B. 错误"));
    	trueFalseQuestions.add(new Question(2, "深圳大学是985。", "B", "A. 正确", "B. 错误"));
    }
    
    // 计时器线程
    private static void Timer() {
        startTime = System.currentTimeMillis();  // 记录计时器开始时间
        while (isRunning) {
            try {
                Thread.sleep(1000);  // 每秒更新一次
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            
            // 计算经过的时间
            elapsedTime = System.currentTimeMillis() - startTime;
            long seconds = elapsedTime / 1000;
            long minutes = seconds / 60;
            seconds = seconds % 60;
            
            // 更新显示的时间
            String timeString = String.format("用时: %02d:%02d", minutes, seconds);
            timerLabel.setText(timeString);
        }
    }
    
    private static void work() {
    	for(int i = 0; i < 15; i++) {
    		try {
    			isSubmitClicked = false;
    			// 展示答案，暂停2秒
    			if(i != 0) {
    		        try {
    		        	submitButton.setEnabled(false);
    		        	TimeUnit.SECONDS.sleep(2);
    		        	submitButton.setEnabled(true);
    		        }
    		        catch (Exception e) {
    					e.getStackTrace();
    				}
    			}
    			giveQuestion(i + 1);
    			
    			// 等待 20 秒，同时检查提交按钮是否被点击
                long startTime = System.currentTimeMillis();
                while (System.currentTimeMillis() - startTime < 20000) {
                    // 检查是否点击了提交按钮
                    if (isSubmitClicked) {
                        break;
                    }
                    // 可以加一点小延时，以减少 CPU 占用
                    Thread.sleep(100);
                }
                
                if(isSubmitClicked) {
                	continue;
                }

                // 如果 20 秒内没有点击提交按钮，则自动提交答案
                submitButton.doClick();
    		}
    		catch (InterruptedException e) {
    			e.printStackTrace();
			}
    	}
    	showResult();
    }
    
    // 随机给出题目
    private static void giveQuestion(int idx) {
		Random rand = new Random();
	    // 随机选择题目类型
		currentQuestiontype = rand.nextInt(3);
		while(finishFlag[currentQuestiontype] == 1) {	// 选择没达到出题上限的题目类型
			currentQuestiontype = rand.nextInt(3);
		}
		
	    if(currentQuestiontype == 0) {
	    	currentQuestion = singleChoiceQuestions.get(rand.nextInt(singleChoiceQuestions.size()));
	    	singleChoiceCount++;
	    }
	    else if(currentQuestiontype == 1) {
	    	currentQuestion = multiChoiceQuestions.get(rand.nextInt(multiChoiceQuestions.size()));
	    	multiChoiceCount++;
	    }
	    else {
	    	currentQuestion = trueFalseQuestions.get(rand.nextInt(trueFalseQuestions.size()));
	    	trueFalseCount++;
	    }
	    
	    // 如果某类题出题达到上限，则打上标记
        if(singleChoiceCount >= 5) finishFlag[0] = 1;
        if(multiChoiceCount >= 5) finishFlag[1] = 1;
        if(trueFalseCount >= 5) finishFlag[2] = 1;
	    
	    // 更新界面显示问题和选项
	    updateQuestionUI(idx);
    }
    
    // 更新界面显示题目
    private static void updateQuestionUI(int idx) {
    	int questionType = currentQuestion.getType();
    	StringBuffer questionBuffer = new StringBuffer();
    	questionBuffer.append(idx + ".(" + questionTypeName[questionType] + ") " + currentQuestion.getQuestionText());
    	
    	// 更新题目标签
        questionLabel.setText(questionBuffer.toString());
        answerPanel.removeAll();
        
        // 根据题目类型创建选项按钮
        if (questionType == 0 || questionType == 2) {
        	ButtonGroup group=new ButtonGroup();
        	for (String option : currentQuestion.getOptions()) {
        		JRadioButton radioButton = new JRadioButton(option);
        		radioButton.setFont(font);
        		group.add(radioButton);
                answerPanel.add(radioButton);
            }
        }
        else {
        	for (String option : currentQuestion.getOptions()) {
                JCheckBox checkBox = new JCheckBox(option);
                checkBox.setFont(font);
                answerPanel.add(checkBox);
            }
        }

        answerPanel.revalidate();
        answerPanel.repaint();
    }
    
    // 提交答案并检查是否正确
    private static void submitAnswer() {
        // 收集用户的答案
        List<String> userAnswerList = new ArrayList<>();
        List<String> correctAnswer = Arrays.asList(currentQuestion.getCorrectAnswers().split(","));
        Component[] components = answerPanel.getComponents();
        for (Component component : components) {
            if (component instanceof JRadioButton && ((JRadioButton) component).isSelected()) {
            	userAnswerList.add(((JRadioButton) component).getText().split("\\.")[0].trim());
            } 
            else if (component instanceof JCheckBox && ((JCheckBox) component).isSelected()) {
            	userAnswerList.add(((JCheckBox) component).getText().split("\\.")[0].trim());
            }
        }
        
        // 检查答案是否正确
        if (userAnswerList.equals(correctAnswer)) {
            // 答对了，更新成绩
        	if(currentQuestiontype == 0) {
                correctSingleChoice++;
                totScore++;
        	}
        	else if(currentQuestiontype == 1) {
                correctMultiChoice++;
                totScore += 2;
        	}
        	else {
                correctTrueFalse++;
                totScore++;
        	}
        }
        
        // 更新界面和统计信息
        updateStatistics();
        
        showCorrectOptions(userAnswerList, correctAnswer);
        
        isSubmitClicked = true;	// 表示已经提交答案
    }
    
    // 展示正确与错误选项
    private static void showCorrectOptions(List<String> userAnswerList, List<String> correctAnswer) {
    	Component[] components = answerPanel.getComponents();
    	
        // 用绿色字体标注正确答案
        for (Component component : components) {
            if (component instanceof JRadioButton) {
                JRadioButton radioButton = (JRadioButton) component;
                String optionText = radioButton.getText().split("\\.")[0].trim();  // 获取选项内容
                if (correctAnswer.contains(optionText)) {
                	radioButton.setForeground(Color.GREEN);  // 正确，字体颜色绿色
                }
            } else if (component instanceof JCheckBox) {
                JCheckBox checkBox = (JCheckBox) component;
                String optionText = checkBox.getText().split("\\.")[0].trim();  // 获取选项内容
                if (correctAnswer.contains(optionText)) {
                	checkBox.setForeground(Color.GREEN);  // 正确，字体颜色绿色
                }
            }
        }
        
        // 用红色字体标注已选择的错误答案
        if(!(userAnswerList.equals(correctAnswer))) {
        	for (Component component : components) {
                if (component instanceof JRadioButton) {
                    JRadioButton radioButton = (JRadioButton) component;
                    String optionText = radioButton.getText().split("\\.")[0].trim();
                    if (userAnswerList.contains(optionText) && !correctAnswer.contains(optionText)) {
                    	radioButton.setForeground(Color.RED);
                    }
                } else if (component instanceof JCheckBox) {
                    JCheckBox checkBox = (JCheckBox) component;
                    String optionText = checkBox.getText().split("\\.")[0].trim();
                    if (userAnswerList.contains(optionText) && !correctAnswer.contains(optionText)) {
                    	checkBox.setForeground(Color.RED);
                    }
                }
            }
        }
        
        // 给出作答结果
        String result = "";
        if(userAnswerList.equals(correctAnswer)) {
        	result = "回答正确";
        }
        else {
        	result = "回答错误";
        }
        resultLabel.setText(result);
        answerPanel.add(resultLabel);
        
        answerPanel.revalidate();
        answerPanel.repaint();
    }
    
    // 更新统计信息
    private static void updateStatistics() {
        String stats = String.format("作答情况： 单选题：%d/%d  多选题：%d/%d  判断题：%d/%d  成绩：%d",
                correctSingleChoice, singleChoiceCount,
                correctMultiChoice, multiChoiceCount,
                correctTrueFalse, trueFalseCount,
                totScore);
        statsLabel.setText(stats);
    }
    
    // 答题结束输出答题整体情况
    private static void showResult() {
        // 计算总用时
    	isRunning = false;
        long totalSeconds = elapsedTime / 1000;
        long minutes = totalSeconds / 60;
        long seconds = totalSeconds % 60;

        // 创建新窗口显示结果
        JFrame resultFrame = new JFrame("考试结果");
        resultFrame.setSize(250, 150);
        resultFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // 创建面板，并设置为垂直布局
        JPanel resultPanel = new JPanel();
        resultPanel.setLayout(new BoxLayout(resultPanel, BoxLayout.Y_AXIS));

        // 创建居中的 JLabel 显示总成绩
        JLabel scoreLabel = new JLabel("总成绩: " + totScore + " 分");
        scoreLabel.setFont(font);
        scoreLabel.setAlignmentX(Component.CENTER_ALIGNMENT);  // 居中对齐
        resultPanel.add(scoreLabel);

        // 创建居中的 JLabel 显示用时
        JLabel timeLabel = new JLabel(String.format("用时: %02d:%02d", minutes, seconds));
        timeLabel.setFont(font);
        timeLabel.setAlignmentX(Component.CENTER_ALIGNMENT);  // 居中对齐
        resultPanel.add(timeLabel);

        // 创建一个居中的按钮
        JButton exitButton = new JButton("退出");
        exitButton.setFont(font);
        exitButton.setAlignmentX(Component.CENTER_ALIGNMENT);  // 居中对齐
        exitButton.addActionListener(e -> System.exit(0));
        resultPanel.add(exitButton);

        // 将面板添加到框架
        resultFrame.add(resultPanel);
        resultFrame.setVisible(true);
    }
}

