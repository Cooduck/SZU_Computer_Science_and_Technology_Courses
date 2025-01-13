#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>

#define White	glm::vec3(1.0, 1.0, 1.0)
#define Yellow	glm::vec3(1.0, 1.0, 0.0)
#define Green	glm::vec3(0.0, 1.0, 0.0)
#define Cyan	glm::vec3(0.0, 1.0, 1.0)
#define Magenta	glm::vec3(1.0, 0.0, 1.0)
#define Red		glm::vec3(1.0, 0.0, 0.0)
#define Black	glm::vec3(0.0, 0.0, 0.0)
#define Blue	glm::vec3(0.0, 0.0, 1.0)
#define Brown	glm::vec3(0.5, 0.5, 0.5)


int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

TriMesh* cube = new TriMesh();

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;


// 关节角
enum{
    Base      = 0,
    LowerArm  = 1,
    UpperArm  = 2,
    NumAngles = 3
};
int Axis = Base;
GLfloat Theta[NumAngles] = {0.0};

// 菜单选项值
const int Quit = 4;

// 尺寸参数
const GLfloat BASE_HEIGHT      = 0.2;
const GLfloat BASE_WIDTH       = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 0.3;
const GLfloat UPPER_ARM_WIDTH  = 0.2;
const GLfloat LOWER_ARM_HEIGHT = 0.4;
const GLfloat LOWER_ARM_WIDTH  = 0.1;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 绘制底座
void base(glm::mat4 modelView)
{
    // 按长宽高缩放正方体，平移至合适位置
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, BASE_HEIGHT / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH));

	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera);
}


// 绘制大臂
void upper_arm(glm::mat4 modelView)
{
	// @TODO: Task-1 参考底座的绘制，在此添加代码绘制大臂
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, UPPER_ARM_HEIGHT / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera);
}


// 绘制小臂
void lower_arm(glm::mat4 modelView)
{
	// @TODO: Task-1 参考底座的绘制，在此添加代码绘制小臂
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, LOWER_ARM_HEIGHT / 2, 0.0));
	instance = glm::scale(instance, glm::vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

	// 绘制，由于我们只有一个立方体数据，所以这里可以直接指定绘制painter中存储的第0个立方体
	painter->drawMesh(0, modelView * instance, light, camera);
}


void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	//创建机器臂的基础立方体
	cube->setNormalize(false);
	cube->generateCube();
	cube->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	cube->setRotation(glm::vec3(0.0, 0.0, 0.0));
	cube->setScale(glm::vec3(1.0, 1.0, 1.0));
	// 加到painter中
	// 指定纹理与着色器，因为不用纹理图片所以就传个空字符串进去了
	painter->addMesh(cube, "Cube", "", vshader, fshader);
	meshList.push_back(cube);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}


void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制底座 
	glm::mat4 modelView = glm::mat4(1.0);
	modelView = glm::translate(modelView, glm::vec3(0.0, -BASE_HEIGHT, 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[Base]), glm::vec3(0.0, 1.0, 0.0));
	base(modelView);

    // @TODO: 在此添加代码完成整个机械手臂绘制 
	// 大臂变换矩阵
	modelView = glm::translate(modelView, glm::vec3(0.0, BASE_HEIGHT, 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[UpperArm]), glm::vec3(0.0, 0.0, 1.0));
 
	// 绘制大臂
	upper_arm(modelView);
 
	// 小臂变换矩阵
	modelView = glm::translate(modelView, glm::vec3(0.0, UPPER_ARM_HEIGHT, 0.0));
	modelView = glm::rotate(modelView, glm::radians(Theta[LowerArm]), glm::vec3(0.0, 0.0, 1.0));
 
	// 绘制小臂    
	lower_arm(modelView);
}


void printHelp()
{
	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<

		"[Part]" << std::endl <<
		"1:		Base" << std::endl <<
		"2:		LowerArm" << std::endl <<
		"3:		UpperArm" << std::endl <<
		std::endl <<

		"[Model]" << std::endl <<
		"a/A:	Increase rotate angle" << std::endl <<
		"s/S:	Decrease rotate angle" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;
		
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: exit(EXIT_SUCCESS); break;
		case GLFW_KEY_Q: exit(EXIT_SUCCESS); break;
		case GLFW_KEY_1: Axis = Base; break;
		case GLFW_KEY_2: Axis = UpperArm; break;
		case GLFW_KEY_3: Axis = LowerArm; break;
		// 通过按键旋转
		case GLFW_KEY_A: 
			Theta[Axis] += 5.0;
			if (Theta[Axis] > 360.0)
				Theta[Axis] -= 360.0;
			break;
		case GLFW_KEY_S:
			Theta[Axis] -= 5.0;
			if (Theta[Axis] < 0.0)
				Theta[Axis] += 360.0;
			break;
		default:
			camera->keyboard(key, action, mode);
			break;
		}
	}
}


void cleanData() {
	delete camera;
	camera = NULL;

	delete light;
	light = NULL;

	painter->cleanMeshes();

	delete painter;
	painter = NULL;
	
	for (int i=0; i<meshList.size(); i++) {
		delete meshList[i];
	}
	meshList.clear();
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
	glfwInit();
	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 设置多重采样，具体的数值可以按照显示效果进行调整。
	glfwWindowHint(GLFW_SAMPLES, 100);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "实验补充一", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	init();
	printHelp();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	while (!glfwWindowShouldClose(window))
	{
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cleanData();
	return 0;
}