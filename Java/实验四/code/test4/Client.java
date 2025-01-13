package test4;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.regex.*;

public class Client {

    private static String serverAddress = "localhost";  // 服务器地址
    private static int serverPort = 12345;  // 服务器端口
    
    // 图形化界面变量
    private static JFrame frame;
    private static JPanel questionPanel;
    private static JLabel questionLabel;
    private static JLabel statsLabel;
    private static JLabel timerLabel;
    private static JPanel answerPanel;
    private static JButton submitButton;
    private static JLabel resultLabel;
    
    // 套接字连接和输出读入端口
    private static Socket socket;
    private static BufferedReader in;
    private static PrintWriter out;
    
    // 字体
    private static Font font = new Font("Label.font", Font.BOLD | Font.PLAIN, 16); // 16是字体大小
    
    // 是否提交答案标记
    private static boolean isSubmitClicked = false;
    
    // 当前问题类型
    private static int currentQuestionType;
    
    // 计时器变量
    private static long startTime;
    private static long elapsedTime;
    private static boolean isRunning = true;

    public static void main(String[] args) {
        try {
            socket = new Socket(serverAddress, serverPort);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            
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
            
            resultLabel = new JLabel("", SwingConstants.LEFT);
            resultLabel.setFont(font);
            
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
            
            // 创建提交按钮
            submitButton = new JButton("提交");
            submitButton.addActionListener(e -> submitAnswer());
            questionPanel.add(submitButton, BorderLayout.SOUTH);
            
            frame.add(questionPanel);
            frame.setVisible(true);
            
            // 启动接收问题的线程和计时线程
            Thread workThread = new Thread(Client::work);
            Thread timeThread = new Thread(Client::Timer);
            workThread.start();
            timeThread.start();

        } catch (IOException e) {
            e.printStackTrace();
        }
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
    			out.println("ready");	// 给服务器发送"ready"信号，告诉服务器已准备接收下一个题目
    			receiveQuestion();	// 获取服务器发送来的题目
    			
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

    // 接收并展示问题
    private static void receiveQuestion() {
        try {
        	answerPanel.removeAll();
        	
            String questionText = in.readLine();
            questionLabel.setText(questionText);
            
            // 使用正则表达式匹配括号内的内容
            Pattern pattern = Pattern.compile("\\((.*?)\\)");  // 匹配 "(" 和 ")" 之间的内容
            Matcher matcher = pattern.matcher(questionText);
            if (matcher.find()) {
            	if(matcher.group(1).equals("单选题")) currentQuestionType = 0;
            	else if(matcher.group(1).equals("多选题")) currentQuestionType = 1;
            	else if(matcher.group(1).equals("判断题")) currentQuestionType = 2;
            }

            String option;
            ButtonGroup buttonGroup = null;

            // 如果是单选题或判断题，则使用单选按钮（JRadioButton）
            if (currentQuestionType == 0 || currentQuestionType == 2) {
                buttonGroup = new ButtonGroup();  // 单选题需要创建一个ButtonGroup
                
                while (!(option = in.readLine()).equals("finish")) {
                    JRadioButton radioButton = new JRadioButton(option);
                    radioButton.setFont(font);
                    buttonGroup.add(radioButton);  // 将单选按钮添加到ButtonGroup
                    answerPanel.add(radioButton);
                }
            } 
            // 如果是多选题，则使用复选框（JCheckBox）
            else if (currentQuestionType == 1) {
                while (!(option = in.readLine()).equals("finish")) {
                    JCheckBox checkBox = new JCheckBox(option);
                    checkBox.setFont(font);
                    answerPanel.add(checkBox);
                }
            }
            
            // 重新绘制面板
            answerPanel.revalidate();
            answerPanel.repaint();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 提交答案
    private static void submitAnswer() {
        try {
        	List<String> selectedAnswers = new ArrayList<>();
            Component[] components = answerPanel.getComponents();
            
            // 筛选出选择的选项
            for (Component component : components) {
                if (component instanceof JRadioButton && ((JRadioButton) component).isSelected()) {
                	selectedAnswers.add(((JRadioButton) component).getText().split("\\.")[0].trim());
                } 
                else if (component instanceof JCheckBox && ((JCheckBox) component).isSelected()) {
                	selectedAnswers.add(((JCheckBox) component).getText().split("\\.")[0].trim());
                }
            }
            if(!selectedAnswers.isEmpty()) {
            	out.println(String.join(",", selectedAnswers));  // 发送答案到服务器
            }
            else {
            	out.println(" ");
            } 
            
            String result = in.readLine();  // 获取服务器返回的答题结果
            resultLabel.setText(result);
            answerPanel.add(resultLabel);	// 展示答题结果
   
            String correctAnswers = in.readLine();	// 获取服务器返回的正确答案
            List<String> correctAnswer = Arrays.asList(correctAnswers.split(","));

            String state = in.readLine();	// 获取服务器返回的答题情况
            statsLabel.setText(state);
            
            // 展示正确和错误选项
            showCorrectOptions(selectedAnswers, correctAnswer);
            answerPanel.revalidate();
            answerPanel.repaint();
            
            isSubmitClicked = true;
        }
        catch (Exception e) {
        	e.printStackTrace();
		}
    }
    
    // 展示正确与错误选项
    private static void showCorrectOptions(List<String> userAnswerList, List<String> correctAnswer) {
    	Component[] components = answerPanel.getComponents();
    	
        // 标注正确答案
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
        
        // 标注已选择的错误答案
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
    }
    
    private static void showResult() {
    	String totScore = "";
    	try {
    		out.println("exam finish");	// 给服务器发送"exam finish"，表示考试结束
        	totScore = in.readLine();
    	}
    	catch (IOException e) {
			e.printStackTrace();
		}
    	
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
