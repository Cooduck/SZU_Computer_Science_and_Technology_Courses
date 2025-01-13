#include "Angel.h"
#include "TriMesh.h"

#include <vector>
#include <string>

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const double DELTA_DELTA = 0.3;		// Delta的变化率
const double DEFAULT_DELTA = 0.4;	// 默认的Delta值

double rotateDelta = DEFAULT_DELTA;

glm::vec3 rotateTheta(0.0, 0.0, 0.0);    // 旋转控制变量

int currentRotateAxis = X_AXIS;	// 设置当前旋转的轴
int stopFlag = 0;	// 控制旋转暂停的标志
int currentRotateDirection = 1;	// 设置旋转方向
int mainWindow;

struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;

	// 着色器程序
	GLuint program;
	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint matrixLocation;
	GLuint darkLocation;
};

openGLObject cow_object;
TriMesh* cow = new TriMesh();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& vshader, const std::string& fshader) {

	// 创建顶点数组对象 (VAO)
    glGenVertexArrays(1, &object.vao);  	// 分配1个顶点数组对象
	glBindVertexArray(object.vao);  		// 绑定顶点数组对象

	// 创建并初始化顶点缓冲对象 (VBO)
	glGenBuffers(1, &object.vbo);			// 创建一个顶点缓冲对象
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo); // 绑定该缓冲对象到GL_ARRAY_BUFFER目标
	glBufferData(GL_ARRAY_BUFFER,			 // 为顶点缓冲对象分配内存并初始化数据
		mesh->getPoints().size() * sizeof(glm::vec3) + mesh->getColors().size() * sizeof(glm::vec3),
		NULL,
		GL_STATIC_DRAW);

	// 将顶点和颜色数据分别上传到缓冲区中
	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->getPoints().size() * sizeof(glm::vec3), &mesh->getPoints()[0]); // 上传顶点数据
	glBufferSubData(GL_ARRAY_BUFFER, mesh->getPoints().size() * sizeof(glm::vec3), mesh->getColors().size() * sizeof(glm::vec3), &mesh->getColors()[0]); // 上传颜色数据

	// 使用指定的着色器文件初始化对象的着色器程序
	object.vshader = vshader;  // 顶点着色器文件
	object.fshader = fshader;  // 片段着色器文件
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str()); // 编译并链接着色器程序

	// 从着色器程序中获取顶点位置的属性位置，并启用该属性
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation); // 启用顶点属性
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // 设置顶点属性指针

	// 从着色器程序中获取顶点颜色的属性位置，并启用该属性
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation); // 启用颜色属性
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mesh->getPoints().size() * sizeof(glm::vec3))); // 设置颜色属性指针

	// 获取矩阵变换的存储位置
	object.matrixLocation = glGetUniformLocation(object.program, "matrix");

}

void init()
{
	std::string vshader, fshader;
	// 读取顶点和片段着色器文件，并为对象绑定数据
    vshader = "shaders/vshader.glsl"; // 顶点着色器路径
	fshader = "shaders/fshader.glsl"; // 片段着色器路径

	cow->readOff(".\\Models\\cow.off"); // 读取OFF文件获取顶点和面片数据
	bindObjectAndData(cow, cow_object, vshader, fshader); // 绑定数据和着色器

	// 设置背景为黑色
	glClearColor(0.0, 0.0, 0.0, 1.0); // 设置清除颜色为黑色
}


void display()
{
	// 清理窗口，准备进行新一帧的绘制，清除颜色和深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 使用绑定的着色器程序
	glUseProgram(cow_object.program);

    // 绑定顶点数组对象（VAO），以便使用其中的数据进行渲染
	glBindVertexArray(cow_object.vao);

	// 初始化4×4的单位变换矩阵，用于进行后续的旋转变换
	glm::mat4 m(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// 执行绕X轴的旋转，角度为rotateTheta[X_AXIS]
	m = glm::rotate(m, glm::radians(rotateTheta[X_AXIS]), glm::vec3(1.0f, 0.0f, 0.0f));	

	// 执行绕Y轴的旋转，角度为rotateTheta[Y_AXIS]
	m = glm::rotate(m, glm::radians(rotateTheta[Y_AXIS]), glm::vec3(0.0f, 1.0f, 0.0f));

	// 执行绕Z轴的旋转，角度为rotateTheta[Z_AXIS]
	m = glm::rotate(m, glm::radians(rotateTheta[Z_AXIS]), glm::vec3(0.0f, 0.0f, 1.0f));

	// 将变换矩阵m传入着色器中对应的uniform变量matrixLocation
	glUniformMatrix4fv(cow_object.matrixLocation, 1, GL_FALSE, glm::value_ptr(m));

	// 绘制模型中的所有三角形，通过使用存储在VAO中的顶点信息
	glDrawArrays(GL_TRIANGLES, 0, cow->getPoints().size());
}


// 通过Delta值更新Theta
void updateTheta(int axis, int sign) {
	rotateTheta[axis] += sign * rotateDelta; // 根据指定轴和方向更新旋转角度
}

// 复原Theta和Delta
void resetTheta()
{
	rotateTheta = glm::vec3(0.0, 0.0, 0.0); // 重置旋转角度为0
	rotateDelta = DEFAULT_DELTA; // 重置变化Delta为默认值
	currentRotateAxis = X_AXIS; // 设置当前旋转的轴为X轴
	stopFlag = 0; // 控制旋转暂停的标志复位
	currentRotateDirection = 1; // 设置旋转方向为正向
}

// 更新变化Delta值
void updateDelta(int sign)
{
	rotateDelta += sign * DELTA_DELTA; // 根据输入的符号更新Delta值
}

// 鼠标按钮回调函数
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // 处理左键点击事件
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            stopFlag = 0; // 左键点击，设置stopFlag为0，表示继续旋转
			std::cout << "旋转继续." << std::endl; 
        }
    } 
    // 处理右键点击事件
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            stopFlag = 1; // 右键点击，设置stopFlag为1，表示暂停旋转
			std::cout << "旋转停止." << std::endl; 
        }
    }
}

// 键盘输入回调函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// 当按键被按下时触发
	if(action == GLFW_PRESS) 
    {
        switch (key)
        {   
            case GLFW_KEY_ESCAPE:	// 按下ESC键，关闭窗口
                glfwSetWindowShouldClose(window, GL_TRUE);
                std::cout << "退出!" << std::endl; 
                break;
            case GLFW_KEY_X:	// 按下X键，设置绕X轴旋转
                currentRotateAxis = X_AXIS;
                std::cout << "绕x轴旋转." << std::endl; 
                break;
            case GLFW_KEY_Y:	// 按下Y键，设置绕Y轴旋转
                currentRotateAxis = Y_AXIS;
                std::cout << "绕y轴旋转." << std::endl; 
                break;
            case GLFW_KEY_Z:	// 按下Z键，设置绕Z轴旋转
                currentRotateAxis = Z_AXIS;
                std::cout << "绕z轴旋转." << std::endl; 
                break;
            case GLFW_KEY_U:	// 按下U键，加快旋转速度
                updateDelta(1);
                std::cout << "加快旋转速度." << std::endl; 
                break;
            case GLFW_KEY_D:	// 按下D键，减慢旋转速度
                updateDelta(-1);
                std::cout << "减慢旋转速度." << std::endl; 
                break;
            case GLFW_KEY_1:	// 按下1键，设置顺时针旋转
                currentRotateDirection = 1;
                std::cout << "顺时针旋转." << std::endl; 
                break;
            case GLFW_KEY_2:	// 按下2键，设置逆时针旋转
                currentRotateDirection = -1;
                std::cout << "逆时针旋转." << std::endl; 
                break;
            case GLFW_KEY_T:	// 按下T键，重置所有旋转参数
                resetTheta();
                std::cout << "重置!" << std::endl; 
                break;
        }
    }
}

// 输出帮助信息
void printHelp() {
	printf("%s\n\n", "3D Transfomations");
	printf("Keyboard options:\n");
	printf("鼠标左键: 旋转动画开始\n");
	printf("鼠标右键: 旋转动画结束\n");
	printf("X: 绕x轴旋转\n");
	printf("Y: 绕y轴旋转\n");
	printf("Z: 绕z轴旋转\n");
	printf("U: 加快旋转速度\n");
	printf("D: 减慢旋转速度\n");
	printf("1: 顺时针旋转\n");
	printf("2: 逆时针旋转\n");
	printf("T: 重置\n");
	printf("Esc: 退出动画\n");
}

void cleanData() {
	cow->cleanData();

	// 释放内存
	delete cow;
	cow = NULL;

	// 删除绑定的对象
    glDeleteVertexArrays(1, &cow_object.vao);

	glDeleteBuffers(1, &cow_object.vbo);
	glDeleteProgram(cow_object.program);
}

int main(int argc, char** argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "实验二", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 设置键盘输入回调函数，当用户按下键盘时调用 key_callback 函数处理输入
	glfwSetKeyCallback(window, key_callback); 

	// 设置鼠标按钮回调函数，当用户按下鼠标按钮时调用 mouseButtonCallback 函数处理输入
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();  // 初始化程序
	printHelp();	// 输出帮助信息
	glEnable(GL_DEPTH_TEST);		// 启用深度测试，保证前后的物体能够正确显示
	
	// 主渲染循环，直到窗口关闭
	while (!glfwWindowShouldClose(window))
	{
		// 如果没有暂停旋转，则更新当前旋转角度
		if(stopFlag == 0){
			updateTheta(currentRotateAxis, currentRotateDirection);
		}
		
		display();  // 调用显示函数进行模型的渲染

		// 交换前后缓冲，显示新一帧画面，并处理输入事件（如键盘、鼠标）
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cleanData();  // 清理和释放资源


	return 0;
}