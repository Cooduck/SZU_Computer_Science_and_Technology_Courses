#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <algorithm>

int WIDTH = 600;
int HEIGHT = 600;

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
	GLuint nLocation;

	// 投影变换变量
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;

	// 阴影变量
	GLuint shadowLocation;
	// 环境光反射、漫反射、镜面反射 控制变量
	GLuint ambientOpen;
	GLuint DiffuseOpen;
	GLuint SpecularOpen;
};

openGLObject mesh_object;
openGLObject plane_object;

Light *light = new Light();

TriMesh *mesh = new TriMesh();

Camera *camera = new Camera();

TriMesh* plane = new TriMesh();

bool isOrtho = true;

void bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader, const std::string &fshader)
{

	// 创建顶点数组对象
	glGenVertexArrays(1, &object.vao); // 分配1个顶点数组对象
	glBindVertexArray(object.vao);	   // 绑定顶点数组对象

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 (mesh->getPoints().size() + mesh->getColors().size() + mesh->getNormals().size()) * sizeof(glm::vec3),
				 NULL,
				 GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->getPoints().size() * sizeof(glm::vec3), &mesh->getPoints()[0]);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->getPoints().size() * sizeof(glm::vec3), mesh->getColors().size() * sizeof(glm::vec3), &mesh->getColors()[0]);
	// @TODO: Task1 修改完TriMesh.cpp的代码成后再打开下面注释，否则程序会报错
	glBufferSubData(GL_ARRAY_BUFFER, (mesh->getPoints().size() + mesh->getColors().size()) * sizeof(glm::vec3), mesh->getNormals().size() * sizeof(glm::vec3), &mesh->getNormals()[0]);

	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

	// 从顶点着色器中初始化顶点的坐标
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 从顶点着色器中初始化顶点的颜色
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mesh->getPoints().size() * sizeof(glm::vec3)));

	// @TODO: Task1 从顶点着色器中初始化顶点的法向量
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((mesh->getPoints().size() + mesh->getColors().size()) * sizeof(glm::vec3)));

	// 获得矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");

	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");

	object.ambientOpen = glGetUniformLocation(object.program, "ambientOpen");
	object.DiffuseOpen = glGetUniformLocation(object.program, "DiffuseOpen");
	object.SpecularOpen = glGetUniformLocation(object.program, "SpecularOpen");
}

void bindLightAndMaterial(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera)
{

	// 传递相机的位置
	glUniform3fv(glGetUniformLocation(object.program, "eye_position"), 1, &camera->eye[0]);

	// 传递物体的材质
	glm::vec4 meshAmbient = mesh->getAmbient();
	glm::vec4 meshDiffuse = mesh->getDiffuse();
	glm::vec4 meshSpecular = mesh->getSpecular();
	float meshShininess = mesh->getShininess();
	glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, &meshAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, &meshDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, &meshSpecular[0]);
	glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);

	// 传递光源信息
	glm::vec4 lightAmbient = light->getAmbient();
	glm::vec4 lightDiffuse = light->getDiffuse();
	glm::vec4 lightSpecular = light->getSpecular();
	glm::vec3 lightPosition = light->getTranslation();

	glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, &lightAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, &lightSpecular[0]);
	glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, &lightPosition[0]);
}

// 鼠标按钮回调函数，处理鼠标左键按下事件
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // 检查是否按下了鼠标左键
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x, y;
        // 获取鼠标当前位置的窗口坐标
        glfwGetCursorPos(window, &x, &y);

        // 获取窗口的宽度和高度，并计算屏幕中心
        float half_winx = WIDTH / 2.0;
        float half_winy = HEIGHT / 2.0;

        // 将鼠标位置从窗口坐标转换为标准化的坐标系 (-1 到 1 范围)
        float lx = float(x - half_winx) / half_winx;  // 计算X轴标准化坐标
        float lz = float(y - half_winy) / half_winy;  // 计算Y轴标准化坐标

        // 获取当前光源的位置
        glm::vec3 pos = light->getTranslation();

        // 更新光源的位置，基于鼠标位置
        pos.x = lx;  // 更新光源的X坐标
        pos.z = lz;  // 更新光源的Z坐标

        // 设置新的光源位置
        light->setTranslation(pos);
    }
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(0.0, 2.0, 0.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0));  // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0));  // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射

	// 设置材质
	mesh->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));  // 环境光
	mesh->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0));  // 漫反射
	mesh->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	mesh->setShininess(100.0);						  //高光系数

	// 设置物体的旋转位移
	mesh->setTranslation(glm::vec3(0.0, 0.0, 0.0));
	mesh->setRotation(glm::vec3(0.0, 0.0, 0.0));
	mesh->setScale(glm::vec3(1.0, 1.0, 1.0));

	// 将物体的顶点数据传递
	bindObjectAndData(mesh, mesh_object, vshader, fshader);

	// 创建正方形平面
	plane->generateSquare(glm::vec3(1.0, 1.0, 1.0));	// 设置颜色
	plane->setRotation(glm::vec3(-90, 0, 0));			// 设置翻转角度
	plane->setTranslation(glm::vec3(0, -0.001, 0));		// 设置位移
	plane->setScale(glm::vec3(5, 5, 5));				// 设置大小
	plane->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));  	// 环境光
	plane->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0));  	// 漫反射
	plane->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); 	// 镜面反射
	plane->setShininess(100.0);						  	//高光系数
	bindObjectAndData(plane, plane_object, vshader, fshader);

	glClearColor(0.5, 0.5, 0.5, 0.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 相机矩阵计算
	camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(isOrtho);

	glBindVertexArray(mesh_object.vao);

	glUseProgram(mesh_object.program);

	// 物体的变换矩阵
	glm::mat4 modelMatrix = mesh->getModelMatrix();

	// 传递矩阵
	glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]);
	// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(mesh_object.shadowLocation, 0);

	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, mesh_object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());


	// 绘制阴影
	glm::mat4 shadowMatrix = light->getShadowProjectionMatrix();  // 获取阴影矩阵
	// 转置阴影矩阵以适应 OpenGL 的列主序存储方式
	shadowMatrix = glm::transpose(shadowMatrix);
	// 将阴影矩阵应用于模型变换
	shadowMatrix = shadowMatrix * modelMatrix;

	// 传递矩阵到着色器
	glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &shadowMatrix[0][0]); // 模型矩阵
	glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]); // 视图矩阵
	glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]); // 投影矩阵

	// 设置阴影标志位为1，表示绘制阴影
	glUniform1i(mesh_object.shadowLocation, 1);
	// 绘制阴影
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size()); // 绘制阴影的几何体


	// 绘制平面
	glBindVertexArray(plane_object.vao); // 绑定平面的顶点数组对象（VAO）
	glUseProgram(plane_object.program);  // 使用平面着色器程序

	modelMatrix = plane->getModelMatrix();  // 获取平面的模型变换矩阵

	// 将模型矩阵、视图矩阵和投影矩阵传递给着色器
	glUniformMatrix4fv(plane_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]); // 模型矩阵
	glUniformMatrix4fv(plane_object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]); // 视图矩阵
	glUniformMatrix4fv(plane_object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]); // 投影矩阵

	// 设置阴影开关，0表示不启用阴影
	glUniform1i(plane_object.shadowLocation, 0);

	// 绑定光源和材质信息
	bindLightAndMaterial(plane, plane_object, light, camera);

	// 绘制平面，使用三角形来绘制平面顶点
	glDrawArrays(GL_TRIANGLES, 0, plane->getPoints().size());
}

void printHelp()
{
	std::cout << "Keyboard Usage" << std::endl;
	std::cout << "[Window]" << std::endl
			  << "ESC:		退出" << std::endl
			  << "h:		打印帮助信息" << std::endl
			  << std::endl
			  << "[Model]" << std::endl
			  << "-:			重置模型材料参数" << std::endl
			  << "1/2/3/!/@/#:	更改模型环境参数" << std::endl
			  << "4/5/6/$/%/^:	更改模型漫反射参数" << std::endl
			  << "7/8/9/&/*/(:	更改模型镜面反射参数" << std::endl
			  << "0/):			改变模型光泽参数" << std::endl
			  << "q:			读入sphere.off" << std::endl
			  << "w:			读入Pikachu.off" << std::endl
			  << "e:			读入Squirtle.off" << std::endl
			  << "r:			读入sphere_coarse.off" << std::endl
			  << std::endl
			  << "[Camera]" << std::endl
			  << "SPACE:	重置相机参数" << std::endl
			  << "u/U:		增加/减少相机的旋转角度" << std::endl
			  << "i/I:		增加/减少相机的向上角度" << std::endl
			  << "o/O:		增加/减少正交投影的范围" << std::endl
			  << "j/J:		增加/减少透视投影下相机与物体之间的距离" << std::endl
			  << "k/K:		增加/减少透视投影的视野" << std::endl
			  << "l/L:		增加/减少透视投影的宽度/高度" << std::endl
			  << "p/P:		切换为正交投影/透视投影（默认为正交投影）" << std::endl
			  << std::endl
			  << "[Light]" << std::endl
			  << "Mouse:	控制灯光位置" << std::endl
			  << "a/A:		增加/减少光在x轴的偏移" << std::endl
			  << "s/S:		增加/减少光在y轴的偏移" << std::endl
			  << "d/D:		增加/减少光在z轴的偏移" << std::endl
			  << std::endl;
}

// 调整材质属性或光源位置的函数
void adjustMaterialProperty(int mode, int property, int componentIndex, float adjustment, 
							float minLimit, float maxLimit) {
	// 如果调整的是高光系数
	if (property == 0) {
		// 获取当前高光系数
		float shininess = mesh->getShininess();
		float tmp = shininess;
		// 根据模式调整高光系数
		if (mode == 0x0000) {
			shininess = std::min(tmp + adjustment, maxLimit);  // 增加高光系数，且不超过最大值
		}
		else if (mode == GLFW_MOD_SHIFT) {
			shininess = std::max(tmp - adjustment, minLimit);  // 减少高光系数，且不低于最小值
		}
		mesh->setShininess(shininess);  // 设置新的高光系数
	}
	// 如果调整的是环境光
	else if (property == 1) {
		glm::vec4 ambient = mesh->getAmbient();
		float tmp = ambient[componentIndex];
		if (mode == 0x0000) {
			ambient[componentIndex] = std::min(tmp + adjustment, maxLimit);  // 增加环境光分量
		}
		else if (mode == GLFW_MOD_SHIFT) {
			ambient[componentIndex] = std::max(tmp - adjustment, minLimit);  // 减少环境光分量
		}
		mesh->setAmbient(ambient);  // 设置新的环境光
	}
	// 如果调整的是漫反射
	else if (property == 2) {
		glm::vec4 diffuse = mesh->getDiffuse();
		float tmp = diffuse[componentIndex];
		if (mode == 0x0000) {
			diffuse[componentIndex] = std::min(tmp + adjustment, maxLimit);  // 增加漫反射分量
		}
		else if (mode == GLFW_MOD_SHIFT) {
			diffuse[componentIndex] = std::max(tmp - adjustment, minLimit);  // 减少漫反射分量
		}
		mesh->setDiffuse(diffuse);  // 设置新的漫反射
	}
	// 如果调整的是镜面反射
	else if (property == 3) {
		glm::vec4 specular = mesh->getSpecular();
		float tmp = specular[componentIndex];
		if (mode == 0x0000) {
			specular[componentIndex] = std::min(tmp + adjustment, maxLimit);  // 增加镜面反射分量
		}
		else if (mode == GLFW_MOD_SHIFT) {
			specular[componentIndex] = std::max(tmp - adjustment, minLimit);  // 减少镜面反射分量
		}
		mesh->setSpecular(specular);  // 设置新的镜面反射
	}
	// 如果调整的是光源的位置
	else if (property == 4) {
		glm::vec3 pos = light->getTranslation();
		float tmp = pos[componentIndex];
		if (mode == 0x0000) {
			pos[componentIndex] = std::min(tmp + adjustment, maxLimit);  // 增加光源位置
		}
		else if (mode == GLFW_MOD_SHIFT) {
			pos[componentIndex] = std::max(tmp - adjustment, minLimit);  // 减少光源位置
		}
		light->setTranslation(pos);  // 设置新的光源位置
	}
}

void mainWindow_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	// @TODO: Task3 添加更多交互 1~9增减反射系数（1~3已写好），0增减高光指数
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		printHelp();
	}
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		std::cout << "读入sphere.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assets/sphere.off");
		init();
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		std::cout << "读入Pikachu.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assets/Pikachu.off");
		init();
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		std::cout << "读入Squirtle.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assets/Squirtle.off");
		init();
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		std::cout << "读入sphere_coarse.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assets/sphere_coarse.off");
		init();
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS){
		if (isOrtho == true){
			isOrtho = false;	// 切换为透视投影
			std::cout << "切换到透视投影" << std::endl;
		}
		else{
			isOrtho = true;	// 切换为正交投影
			std::cout << "切换到正交投影" << std::endl;
		}

	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) adjustMaterialProperty(mode, 1, 0, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) adjustMaterialProperty(mode, 1, 1, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS) adjustMaterialProperty(mode, 1, 2, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS) adjustMaterialProperty(mode, 2, 0, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS) adjustMaterialProperty(mode, 2, 1, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_6 && action == GLFW_PRESS) adjustMaterialProperty(mode, 2, 2, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS) adjustMaterialProperty(mode, 3, 0, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS) adjustMaterialProperty(mode, 3, 1, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS) adjustMaterialProperty(mode, 3, 2, 0.1f, 0.0f, 1.0f);
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS) adjustMaterialProperty(mode, 0, 0, 10.0f, 0.0f, 200.0f);
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) adjustMaterialProperty(mode, 4, 0, 0.1f, -10.0f, 10.0f);
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) adjustMaterialProperty(mode, 4, 1, 0.1f, 1.5f, 10.0f);
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) adjustMaterialProperty(mode, 4, 2, 0.1f, -10.0f, 10.0f);
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS && mode == 0x0000)
	{
		mesh->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));
		mesh->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0));
		mesh->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0));
		mesh->setShininess(100.0);
	}
	else
	{
		camera->keyboard(key, action, mode);
	}
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData()
{
	mesh->cleanData();

	delete camera;
	camera = NULL;

	// 释放内存
	delete mesh;
	mesh = NULL;

	// 删除绑定的对象
	glDeleteVertexArrays(1, &mesh_object.vao);
	glDeleteBuffers(1, &mesh_object.vbo);
	glDeleteProgram(mesh_object.program);
}

int main(int argc, char **argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow *mainwindow = glfwCreateWindow(600, 600, "实验三", NULL, NULL);
	if (mainwindow == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainwindow);
	glfwSetFramebufferSizeCallback(mainwindow, framebuffer_size_callback);
	glfwSetKeyCallback(mainwindow, mainWindow_key_callback);
	glfwSetMouseButtonCallback(mainwindow, mouse_button_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	mesh->readOff("./assets/sphere.off");
	mesh->generateCube();
	// Init mesh, shaders, buffer
	init();
	printHelp();
	// bind callbacks
	while (!glfwWindowShouldClose(mainwindow))
	{

		display();
		glfwSwapBuffers(mainwindow);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
