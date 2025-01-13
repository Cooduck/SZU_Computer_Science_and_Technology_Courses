#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>

int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// 读取着色器并使用
std::string vshader = "shaders/vshader.glsl";
std::string fshader = "shaders/fshader.glsl";

// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;

float lastX = 300.0;  // 上次鼠标的X坐标
float lastY = 300.0;	// 上次鼠标的Y坐标
bool isCursorVisible = false;	// 用来追踪鼠标的当前状态
GLFWwindow* window;

glm::vec3 lightPos = glm::vec3(0.0, 10.0, 0.0);

class MatrixStack {
	int		_index;
    int		_size;
    glm::mat4*	_matrices;

public:
	MatrixStack(int numMatrices = 100):_index(0), _size(numMatrices)
        { _matrices = new glm::mat4[numMatrices]; }

    ~MatrixStack()
		{ delete[]_matrices; }

    void push(const glm::mat4& m){
		assert( _index + 1 < _size );
		_matrices[_index++] = m;	 
    }

	glm::mat4& pop(){
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};

// 定义treenode结构体，用于表示层级化的节点（物体的各个部位）
struct treenode
{
    int nodeId;                     // 节点的唯一标识符
    glm::vec3 translation;          // 该节点的平移变换
    glm::vec3 rotation;             // 该节点的旋转变换
    glm::vec3 scale;                // 该节点的缩放变换
    struct treenode *child;         // 指向子节点的指针
    struct treenode *sibling;       // 指向兄弟节点的指针
    glm::mat4 selfTransformM = glm::mat4(1.0); // 该节点的局部变换矩阵（初始为单位矩阵）
    bool drawFlag = true;           // 是否绘制该节点的标志（默认绘制）

    // 构造函数，初始化treenode的各个成员
    treenode(int nodeId, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, 
             treenode *child, treenode *sibling, glm::mat4 selfTransformM){
        this->nodeId = nodeId;
        this->translation = translation;
        this->rotation = rotation;
        this->scale = scale;
        this->child = child;
        this->sibling = sibling;
        this->selfTransformM = selfTransformM;
    }
};

// 声明 Steve 角色各部位的节点
treenode* steveRightLeg;
treenode* steveLeftLeg;
treenode* steveRightArm;
treenode* steveLeftArm;
treenode* steveHead;
treenode* steveBody;

struct Steve
{
	// steve 各部位
	enum {
		Body,			// 躯干
		Head,			// 头部
		LeftArm,		// 左臂
		RightArm,		// 右臂
		LeftLeg,		// 左腿
		RightLeg,		// 右腿
	};

	// steve 各部位旋转方向
	int roateDir[6] = {
		1,    // 躯干
		1,    // 头部
		1,    // 左臂
		-1,    // 右臂
		-1,    // 左腿
		1,    // 右腿
	};

	// steve 各部位名字
	std::string modelNames[6] = {
		"steveBody",
		"steveHead",
		"steveArm",
		"steveArm",
		"steveLeg",
		"steveLeg"
	};

	TriMesh* triMesh[6];
	openGLObject object[6];
	glm::vec3 stevePos = glm::vec3(0.0, 1.0, 0.0);	// steve 的位置
	glm::vec3 steveDir;	// steve 的方向
	float speed = 0.1f;	// steve 的移动速度
	bool jumping = false;	// steve 是否跳跃的标志
	int jumpingDirection = 1;	// steve 的跳跃方向
	bool waving = false;	// steve 是否挥手的标志
	int wavingState = 0;	// steve 挥手的状态

	Steve(){
		for(int i = 0; i < 6; ++i){
			triMesh[i] = new TriMesh();
			std::string objPath = "./assets/" + modelNames[i] + "/" + modelNames[i] + ".obj";	// 模型路径

			triMesh[i]->setNormalize(true);	// 物体大小归一化
			triMesh[i]->readObj(objPath);	// 读取模型
			triMesh[i]->setTranslation(glm::vec3{0.0, 0.0, 0.0});	// 设置位移
			triMesh[i]->setRotation(glm::vec3{0.0, 0.0, 0.0});	// 设置旋转
			triMesh[i]->setScale(glm::vec3{1.0, 1.0, 1.0});	// 设置缩放
			triMesh[i]->setAmbient(glm::vec4{0.0, 0.0, 0.0, 0.0});  // 环境光
			triMesh[i]->setDiffuse(glm::vec4{0.2, 0.2, 0.2, 1.0});  // 漫反射
			triMesh[i]->setSpecular(glm::vec4{0.5, 0.5, 0.5, 1.0}); // 镜面反射
			triMesh[i]->setShininess(225);	//高光系数
		}
		for(int i = 0; i < 6; ++i) 
			painter->bindObjectAndData(triMesh[i], object[i], "./assets/" + modelNames[i] + "/steve.jpg", vshader, fshader);
	}

	// steve 的跳跃动画
	void updateJumpAnimation() {
		if (jumping) {

			// 根据跳跃方向更新 rotation[1]
			stevePos[1] += jumpingDirection * 0.1f;

			// 如果跳跃动画完成，切换状态
			if (stevePos[1] >= 1.5f || stevePos[1] <= 1.0f) {
				if (jumpingDirection == 1 && stevePos[1] >= 1.5f) {
					jumpingDirection = -1;  // 从 1 回到 0
				}
				else if (jumpingDirection == -1 && stevePos[1] <= 1.0f) {
					jumping = false;  // 跳跃结束
				}
			}
		}
	}

	// steve 的挥手动画，控制右臂往特定方向挥动
	void updateWavingAnimation(){
		if (waving) {
			if(camera->getPerspectiveMode() != 3){	// 非第一人称下 steve 的挥手动画
				if(wavingState == 0){
					steveRightArm->rotation = glm::vec3(steveRightArm->rotation[0] - 22.5, steveRightArm->rotation[1], steveRightArm->rotation[2] - 15.0);
					if(steveRightArm->rotation[0] <= -90.0 && steveRightArm->rotation[2] <= -60.0){
						wavingState = 1;
					}
				}
				else if(wavingState == 1){
					steveRightArm->rotation = glm::vec3(steveRightArm->rotation[0], steveRightArm->rotation[1], steveRightArm->rotation[2] + 30.0);
					if(steveRightArm->rotation[2] >= 60.0){
						wavingState = 2;
					}
				}
				else if(wavingState == 2){
					steveRightArm->rotation = glm::vec3(steveRightArm->rotation[0] + 22.5, steveRightArm->rotation[1], steveRightArm->rotation[2] - 15.0);
					if(steveRightArm->rotation[0] >= 0.0 && steveRightArm->rotation[2] <= 0.0){
						wavingState = 0;
						waving = false;
						steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
					}
				}
			}
			else{
				if(wavingState == 0){	// 第一人称下 steve 的挥手动画
					steveRightArm->rotation[0] -= 5.0;
					if(steveRightArm->rotation[0] <= -135.0){
						wavingState = 1;
					}
				}
				else if(wavingState == 1){
					steveRightArm->rotation[0] += 5.0;
					if(steveRightArm->rotation[0] >= -120.0){
						wavingState = 0;
						waving = false;
						steveRightArm->translation = glm::vec3{-0.23, -0.3, 0.0};
						steveRightArm->rotation = glm::vec3{-120.0, 0.0, -30.0};
					}
				}
			}
		}
	}
};

Steve* steve;

struct Creeper{
	TriMesh* triMesh;
	openGLObject object;
	glm::vec3 creeperPos = glm::vec3{-2.0, 0.67, 0.0};	// creeperPos 的位置
	glm::vec3 creeperDir = glm::vec3{1.0, 0.0, 0.0};	// creeperDir 的方向
	bool attacked = false;	// creeper 是否被攻击的标志
	glm::vec3 attackedDir;	// creeper 被攻击的方向
	int jumpingDirection = 1;	// creeper 跳跃方向
	bool dontMove = false;	// creeper 是否移动的标志

	Creeper(){
		triMesh = new TriMesh();
		std::string objPath = "./assets/creeper/creeper.obj";
		std::string picturePath = "./assets/creeper/creeper.jpg";

		triMesh->setNormalize(true);	// 物体大小归一化
		triMesh->readObj(objPath);	// 读取模型

		// 设置物体的旋转位移
		triMesh->setTranslation(glm::vec3{-2.0, 0.67, 0.0});	// 设置位移
		triMesh->setRotation(glm::vec3{0.0, 0.0, 0.0});	// 设置旋转
		triMesh->setScale(glm::vec3{1.5, 1.5, 1.5});	// 设置缩放
		triMesh->setAmbient(glm::vec4{0.0, 0.0, 0.0, 0.0});  // 环境光
		triMesh->setDiffuse(glm::vec4{0.2, 0.2, 0.2, 1.0});  // 漫反射
		triMesh->setSpecular(glm::vec4{0.5, 0.5, 0.5, 1.0}); // 镜面反射
		triMesh->setShininess(225);	//高光系数
		triMesh->setDrawShade(true);	// 设置是否绘制阴影

		painter->bindObjectAndData(triMesh, object, picturePath, vshader, fshader);
	}

	// creeper 的被攻击动画
	void updateCreeperAttacked(){
		if (attacked) {
			triMesh->setAmbient(glm::vec4{0.8f, 0.0f, 0.0f, 1.0f});  // 漫反射设为红色
			triMesh->setDiffuse(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});  // 漫反射设为红色
			// 根据跳跃方向更新 rotation[1]
			creeperPos[1] += jumpingDirection * 0.1f;
			// 如果跳跃动画完成，切换状态
			if (creeperPos[1] >= 1.17f || creeperPos[1] <= 0.67f) {
				if (jumpingDirection == 1 && creeperPos[1] >= 1.17f) {
					jumpingDirection = -1;  // 跳跃到一定高度则下落
				}
				else if (jumpingDirection == -1 && creeperPos[1] <= 0.67f) {
					// 恢复为最初的状态
					attacked = false; 
					jumpingDirection = 1;
					triMesh->setAmbient(glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});
					triMesh->setDiffuse(glm::vec4{0.2, 0.2, 0.2, 1.0});
				}
			}

			creeperPos += attackedDir * glm::vec3(0.15, 0.0, 0.15);	// 沿着被攻击方向位移
		}
		triMesh->setTranslation(creeperPos);	// 更新 creeper 位置
	}

	// 如果 creeper 可以移动，则向 steve 靠近
	void updateCreeperPos(){
		// 如果 creeper 没有被限制移动并且没有处于被攻击状态
		if(dontMove == false && attacked == false){
			// 计算 creeper 和 steve 之间的方向向量（平面上的 x 和 z 分量）
			creeperDir = glm::vec3(steve->stevePos.x - creeperPos.x, 0.0, steve->stevePos.z - creeperPos.z);
			
			// 计算 creeper 朝向 steve 的角度（弧度制），并将其转换为角度制
			float angleRad = atan2(creeperDir.z, creeperDir.x);
			float angleDeg = angleRad * (180.0f / M_PI);
			
			// 如果角度为负值，将其转换为正角度
			if (angleDeg < 0) {
				angleDeg += 360.0f;
			}

			// 设置 creeper 的旋转角度，使其面向 steve
			triMesh->setRotation(glm::vec3{0.0, -angleDeg, 0.0});

			// 计算 creeper 和 steve 之间的水平距离
			float distance = sqrt(pow((steve->stevePos.x - creeperPos.x), 2) + pow((steve->stevePos.z - creeperPos.z), 2));

			// 如果距离 steve 还有一定的间距，则 creeper 向 steve 移动
			if(distance >= 1.0){
				// creeper 按照方向向量的方向移动一定距离
				creeperPos += creeperDir * glm::vec3(0.01, 0.0, 0.01);
			}
		}
		
		// 更新 creeper 的新位置
		triMesh->setTranslation(creeperPos);
	}

};

Creeper* creeper;

struct Point{
	float points[2] = {0.0f, 0.0f};
	GLuint vao, vbo;
	Point(){
		glGenVertexArrays(1, &vao);  	// 分配1个顶点数组对象
		glBindVertexArray(vao);  	// 绑定顶点数组对象
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

		// 配置顶点属性指针
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 解绑VAO和VBO
		glUseProgram(0);
		glBindVertexArray(0);
	}

	void drawPoint(){
		// 在屏幕中心绘制一个点
		glBindVertexArray(vao);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 1);
	}
};

Point *point;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 计算两个向量之间的夹角
float angleBetweenVectors(const glm::vec3& v1, const glm::vec3& v2) {
    // 计算点积
    float dotProduct = glm::dot(v1, v2);

    // 计算模长
    float magnitude1 = glm::length(v1);
    float magnitude2 = glm::length(v2);

    // 计算余弦值
    float cosTheta = dotProduct / (magnitude1 * magnitude2);

    // 由于浮动误差，确保余弦值在 -1 到 1 之间
    cosTheta = glm::clamp(cosTheta, -1.0f, 1.0f);

    // 计算夹角（弧度）
    float angleRad = glm::acos(cosTheta);

    // 将弧度转换为角度
    float angleDeg = glm::degrees(angleRad);

	if(v2.y != 0.0 && v2.y > 0.0) return -angleDeg;
    else return angleDeg;
}

void addTriMesh(const std::string &modelName, const glm::vec3 &translation, const glm::vec3 &rotation, 
				const glm::vec3 &scale, const glm::vec4 &ambient, const glm::vec4 &diffuse, 
				const glm::vec4 &specular, const float &Shininess, const bool &drawShade){
	TriMesh* trimesh = new TriMesh();
	std::string objPath = "./assets/" + modelName + "/" + modelName + ".obj";	// obj 模型路径
	std::string picturePath = "./assets/" + modelName + "/" + modelName + ".jpg";	// obj 模型材质路径

	trimesh->setNormalize(true);	// 归一化物体大小	
	trimesh->readObj(objPath);		// 读取 obj 模型

	trimesh->setTranslation(translation);	// 设置位移
	trimesh->setRotation(rotation);	// 设置旋转
	trimesh->setScale(scale);	// 设置缩放
	trimesh->setAmbient(ambient);  // 环境光
	trimesh->setDiffuse(diffuse);  // 漫反射
	trimesh->setSpecular(specular); // 镜面反射
	trimesh->setShininess(Shininess);	//高光系数
	trimesh->setDrawShade(drawShade);	// 设置是否绘制阴影

	// 加到painter中
	painter->addMesh(trimesh, modelName, picturePath, vshader, fshader); 	// 指定纹理与着色器
}

void init()
{
	// 设置光源位置
	light->setTranslation(lightPos);
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	light->setAttenuation(1.0, 0.045, 0.0075); // 衰减系数
	
	// 创建 20x20 的草方块地面
	for (int i = 0; i < 50; ++i) {
		for (int j = 0; j < 50; ++j) {
			// 每个草方块设置位置
			glm::vec3 translation = glm::vec3(-17.5 + i*0.7, -0.01, -17.5 + j*0.7);
			addTriMesh("grass", translation, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{1.0, 1.0, 1.0}, 
				glm::vec4{0.0, 0.0, 0.0, 1.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.039456, 0.039456, 0.039456, 1.0}, 96.078431, false);
		}
	}

	// 创建 steve
	steve = new Steve();

	// 构建 steve 每个部位的层级节点
	steveRightLeg = new treenode(steve->RightLeg, glm::vec3(-0.13, -0.71, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.9, 0.9, 0.9),
											NULL, NULL, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0)));
	steveLeftLeg = new treenode(steve->LeftLeg, glm::vec3(0.13, -0.71, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.9, 0.9, 0.9),
											NULL, steveRightLeg, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0)));
	steveRightArm = new treenode(steve->RightArm, glm::vec3(-0.37, -0.03, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.75, 0.75, 0.75),
											NULL, steveLeftLeg, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0)));
	steveLeftArm = new treenode(steve->LeftArm, glm::vec3(0.37, -0.03, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.75, 0.75, 0.75),
											NULL, steveRightArm, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0)));
	steveHead = new treenode(steve->Head, glm::vec3(0.0, 0.32, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.7, 0.7, 0.7),
											NULL, steveLeftArm, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, 0.0)));
	steveBody = new treenode(steve->Body, steve->stevePos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.7, 0.7, 0.7), steveHead, 
											NULL, glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0)));
	// 创建 creeper
	creeper = new Creeper();

	// 创建天空盒
	addTriMesh("sky", glm::vec3{0.0, 0.0, 0.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{200.0, 200.0, 200.0}, 
				glm::vec4{0.0, 0.0, 0.0, 0.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.0, 0.0, 0.0, 1.0}, 225, false);
	
	// 创建 vilager
	addTriMesh("vilager", glm::vec3{2.0, 0.62, 0.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{1.5, 1.5, 1.5}, 
				glm::vec4{0.0, 0.0, 0.0, 0.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.5, 0.5, 0.5, 1.0}, 225, true);
	
	// 创建 VoxelHouse_01
	addTriMesh("VoxelHouse_01", glm::vec3{-4.0, 3.77, 4.5}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{12.0, 12.0, 12.0}, 
				glm::vec4{0.0, 0.0, 0.0, 0.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.5, 0.5, 0.5, 1.0}, 96.078431, true);

	// 创建 VoxelHouse_02
	addTriMesh("VoxelHouse_02", glm::vec3{6.0, 1.9, -5.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{12.0, 12.0, 12.0}, 
				glm::vec4{0.0, 0.0, 0.0, 0.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.5, 0.5, 0.5, 1.0}, 96.078431, true);
	
	// 创建 VoxelHouse_02
	addTriMesh("VoxelHouse_03", glm::vec3{-6.0, 2.15, -5.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{12.0, 12.0, 12.0}, 
				glm::vec4{0.0, 0.0, 0.0, 0.0}, glm::vec4{0.2, 0.2, 0.2, 1.0}, glm::vec4{0.5, 0.5, 0.5, 1.0}, 96.078431, true);

	// 创建点
	point = new Point();

	glClearColor(0.68f, 0.85f, 0.9f, 1.0f);
}

// 递归遍历层级结构并进行绘制
void traverse(MatrixStack &mstack, treenode *node, glm::mat4 modelMatrix)
{
    if(node == NULL) return;  // 如果节点为空，则返回

    mstack.push(modelMatrix);  // 将当前的模型矩阵入栈，保存当前状态

    // 对当前节点应用平移、旋转、缩放变换
    modelMatrix = glm::translate(modelMatrix, node->translation);  // 平移
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->rotation[2]), glm::vec3(0.0, 0.0, 1.0)); // 绕Z轴旋转
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->rotation[1]), glm::vec3(0.0, 1.0, 0.0)); // 绕Y轴旋转
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->rotation[0]), glm::vec3(1.0, 0.0, 0.0)); // 绕X轴旋转
    modelMatrix = glm::scale(modelMatrix, node->scale);  // 缩放

    // 设置当前节点的材质属性
    steve->triMesh[node->nodeId]->setAmbient(glm::vec4{0.0, 0.0, 0.0, 0.0});  // 环境光
    steve->triMesh[node->nodeId]->setDiffuse(glm::vec4{0.2, 0.2, 0.2, 1.0});  // 漫反射
    steve->triMesh[node->nodeId]->setSpecular(glm::vec4{0.5, 0.5, 0.5, 1.0}); // 镜面反射
    steve->triMesh[node->nodeId]->setShininess(225); // 高光系数

    // 如果当前节点的绘制标志为true，则绘制该节点
    if(node->drawFlag == true) 
        painter->drawMeshWithM(steve->triMesh[node->nodeId], modelMatrix * node->selfTransformM, steve->object[node->nodeId], light, camera);

    // 如果节点有子节点，递归遍历子节点
    if(node->child != NULL) 
        traverse(mstack, node->child, modelMatrix);

    modelMatrix = mstack.pop();  // 弹出栈顶的矩阵，恢复之前的状态

    // 如果节点有兄弟节点，递归遍历兄弟节点
    if(node->sibling != NULL) 
        traverse(mstack, node->sibling, modelMatrix);
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	painter->drawMeshes(light, camera);	// 构建静态虚拟场景

	painter->drawMesh(creeper->triMesh, creeper->object, light, camera);	// 构建 creeper


    glm::mat4 modelMatrix = glm::mat4(1.0);  // 初始化物体的变换矩阵（单位矩阵）
    MatrixStack mstack;  // 创建变换矩阵栈，用于保存中间状态
    traverse(mstack, steveBody, modelMatrix);  // 递归地进行层级建模和渲染

	// 更新 steve 的方向、位移
	steve->steveDir = glm::vec3(cos(glm::radians(steveHead->rotation[0])) * sin(glm::radians(steveBody->rotation[1])),
								sin(glm::radians(steveHead->rotation[0])), 
								cos(glm::radians(steveHead->rotation[0])) * cos(glm::radians(steveBody->rotation[1])));
	steveBody->translation = steve->stevePos;

	// 更新相机中记录的 steve 的方向、位移
	camera->stevePos = steve->stevePos, camera->steveDir = steve->steveDir;

	// 当为第一人称视角时，在屏幕中央画一个点
	if(camera->getPerspectiveMode() == 3){
		point->drawPoint();
	}
}


void printHelp()
{
    std::cout << "================================================" << std::endl;
    std::cout << "Use keyboard and mouse to interact with the scene." << std::endl;
    std::cout << "================================================" << std::endl << std::endl;

    std::cout << "Keyboard Usage" << std::endl;
    std::cout <<
        "[General]" << std::endl <<
        "ESC:        Exit program" << std::endl <<
        "H:          Print help message" << std::endl << std::endl <<

        "[Steve Movement]" << std::endl <<
        "W/A/S/D:    Move Steve forward/left/backward/right" << std::endl <<
        "Shift:      Sprint (accelerated movement)" << std::endl <<
        "Space:      Make Steve jump" << std::endl << std::endl <<

        "[Camera]" << std::endl <<
        "1:          Global view" << std::endl <<
        "2:          Third-person view (behind Steve)" << std::endl <<
        "3:          First-person view (Steve's head)" << std::endl <<
        "4:          Third-person view (in front of Steve)" << std::endl <<
        "5:          Free view (move camera freely)" << std::endl << std::endl <<

		"[Light]" << std::endl <<
        "J/K/L:      Adjust light position (x/y/z axis)" << std::endl << std::endl <<

        "[Creeper]" << std::endl <<
        "M:          Toggle Creeper movement" << std::endl << std::endl <<

		"[Free View]" << std::endl <<
        "W/A/S/D:    Move Camera forward/left/backward/right" << std::endl << std::endl << 

		"[Other]" << std::endl <<
		"N:          Toggle mouse visibility" << std::endl <<
        "Space:      Reset camera parameters" << std::endl << std::endl;

    std::cout << "Mouse Usage" << std::endl;
    std::cout <<
        "[General]" << std::endl <<
        "Left Click: Trigger Steve's waving animation or attack Creeper (if in range)" << std::endl << std::endl <<

        "[Global View]" << std::endl <<
        "Drag:       Rotate the view around the scene" << std::endl <<
        "Scroll:     Zoom in/out of the scene" << std::endl << std::endl <<

        "[Third-Person View 1]" << std::endl <<
        "Horizontal Drag: Rotate around Steve from behind" << std::endl <<
        "Vertical Drag:   Adjust viewing angle from above/below" << std::endl << std::endl <<

        "[First-Person View]" << std::endl <<
        "Horizontal Drag: Turn Steve's head left/right" << std::endl <<
        "Vertical Drag:   Look up/down" << std::endl << std::endl <<

        "[Third-Person View 2]" << std::endl <<
        "Horizontal Drag: Rotate around Steve from the front" << std::endl <<
        "Vertical Drag:   Adjust viewing angle from above/below" << std::endl << std::endl <<

        "[Free View]" << std::endl <<
        "Drag:       Move the camera's orientation" << std::endl <<
        "Scroll:     Adjust camera height" << std::endl << std::endl;
}


// key_callback 只处理按键的按下和释放事件
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	bool isShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {	// 退出
            exit(EXIT_SUCCESS);
        }
        else if (key == GLFW_KEY_H) {	// 帮助
            printHelp();
        }
		else if((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) && 
					action == GLFW_PRESS && camera->getPerspectiveMode() != 5){
			// 重置 steve 手脚挥动方向
			steve->roateDir[2] = 1, steve->roateDir[3] = -1, steve->roateDir[4] = -1, steve->roateDir[5] = 1;
		}
        else if (key == GLFW_KEY_SPACE) {
			// 触发 steve 的跳跃动画
            if (!steve->jumping) {
                steve->jumping = true;
                steve->jumpingDirection = 1;
            }
        }
        else if (key == GLFW_KEY_1) {	// 切换为全局视角
            camera->changePerspectiveMode(1);
			steveLeftArm->sibling = steveRightArm;
			steveRightArm->sibling = steveLeftLeg;
			steveRightArm->translation = glm::vec3{-0.37, -0.03, 0.0};
            steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
            steveRightArm->scale = glm::vec3(0.75, 0.75, 0.75);
			steve->triMesh[steveRightArm->nodeId]->setDrawShade(true);
			steveHead->translation = glm::vec3(0.0, 0.32, 0.0);
			steveHead->selfTransformM = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, 0.0));
			steveHead->child = NULL;
            steveHead->drawFlag = true;
        }
        else if (key == GLFW_KEY_2) {	// 切换为第三人称视角1
            camera->changePerspectiveMode(2);
            steveLeftArm->sibling = steveRightArm;
			steveRightArm->sibling = steveLeftLeg;
			steveRightArm->translation = glm::vec3{-0.37, -0.03, 0.0};
            steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
            steveRightArm->scale = glm::vec3(0.75, 0.75, 0.75);
			steve->triMesh[steveRightArm->nodeId]->setDrawShade(true);
			steveHead->translation = glm::vec3(0.0, 0.32, 0.0);
			steveHead->selfTransformM = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, 0.0));
			steveHead->child = NULL;
            steveHead->drawFlag = true;
            lastX = 300.0;
            lastY = 300.0;
        }
        else if (key == GLFW_KEY_3) {	// 切换为第一人称视角
            camera->changePerspectiveMode(3);
			steveLeftArm->sibling = steveLeftLeg;
			steveRightArm->sibling = NULL;
			steveRightArm->translation = glm::vec3{-0.23, -0.3, 0.0};
            steveRightArm->rotation = glm::vec3{-120.0, 0.0, -30.0};
            steveRightArm->scale = glm::vec3{0.35, 0.75, 0.35};
			steve->triMesh[steveRightArm->nodeId]->setDrawShade(false);
			steveHead->translation = glm::vec3(0.0, 0.49, 0.0);
			steveHead->selfTransformM = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.35, 0.0));
			steveHead->child = steveRightArm;
            steveHead->drawFlag = false;
            lastX = 300.0;
            lastY = 300.0;
        }
		else if (key == GLFW_KEY_4){	// 切换为第三人称视角2
			camera->changePerspectiveMode(4);
            steveLeftArm->sibling = steveRightArm;
			steveRightArm->sibling = steveLeftLeg;
			steveRightArm->translation = glm::vec3{-0.37, -0.03, 0.0};
            steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
            steveRightArm->scale = glm::vec3(0.75, 0.75, 0.75);
			steve->triMesh[steveRightArm->nodeId]->setDrawShade(true);
			steveHead->translation = glm::vec3(0.0, 0.32, 0.0);
			steveHead->selfTransformM = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, 0.0));
			steveHead->child = NULL;
            steveHead->drawFlag = true;
			lastX = 300.0;
            lastY = 300.0;
		}
		else if (key == GLFW_KEY_5) {	// 切换为自由视角
            camera->changePerspectiveMode(5);
			camera->initCameraMode5();
			steveLeftArm->sibling = steveRightArm;
			steveRightArm->sibling = steveLeftLeg;
			steveRightArm->translation = glm::vec3{-0.37, -0.03, 0.0};
            steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
            steveRightArm->scale = glm::vec3(0.75, 0.75, 0.75);
			steve->triMesh[steveRightArm->nodeId]->setDrawShade(true);
			steveHead->translation = glm::vec3(0.0, 0.32, 0.0);
			steveHead->selfTransformM = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, 0.0));
			steveHead->child = NULL;
            steveHead->drawFlag = true;
			lastX = 300.0;
            lastY = 300.0;
        }
		else if(key == GLFW_KEY_J){	// 控制光照沿着 x 轴移动
			if (!isShiftPressed) {
				lightPos[0] += 1.0;
            } 
			else {
				lightPos[0] -= 1.0;
            }
			light->setTranslation(lightPos);
		}
		else if(key == GLFW_KEY_K){	// 控制光照沿着 y 轴移动
			if (!isShiftPressed) {
				lightPos[1] = glm::min(lightPos[1] + 1.0f, 40.0f);
            } 
			else {
				lightPos[1] = glm::max(lightPos[1] - 1.0f, 10.0f);
            }
			light->setTranslation(lightPos);
		}
		else if(key == GLFW_KEY_L){	// 控制光照沿着 z 轴移动
			if (!isShiftPressed) {
				lightPos[2] += 1.0;
            } 
			else {
				lightPos[2] -= 1.0;
            }
			light->setTranslation(lightPos);
		}
		else if(key == GLFW_KEY_N){	// 控制鼠标的显示
            if (isCursorVisible) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // 隐藏鼠标
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // 显示鼠标
            }
            isCursorVisible = !isCursorVisible;  // 切换鼠标状态
        }
		else if(key == GLFW_KEY_M){	// 控制 creeper 是否移动
			creeper->dontMove = !creeper->dontMove;
		}
        else {	// 调用相机键盘响应函数
            camera->keyboard(key, action, mode);
        }
    }
    // 按键松开时的操作
    else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
            // 松开方向键时，恢复为 steve 的站立状态
            steveLeftArm->rotation[0] = 0.0;
            if (camera->getPerspectiveMode() != 3) {
                steveRightArm->rotation = glm::vec3{0.0, 0.0, 0.0};
            }
            else {	// 第一视角时要保持右臂的举起
				steveRightArm->translation = glm::vec3{-0.23, -0.3, 0.0};
                steveRightArm->rotation = glm::vec3{-120.0, 0.0, -30.0};
            }
            steveLeftLeg->rotation[0] = 0.0;
            steveRightLeg->rotation[0] = 0.0;
        }
    }
}


// 控制 steve 走路、跑步
void steveMove() {
    // 持续检测按键状态
    bool isShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    bool isSpacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    bool isWPressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    bool isSPressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    bool isAPressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    bool isDPressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);

    if ((isWPressed || isSPressed || isAPressed || isDPressed) && camera->getPerspectiveMode() != 5) {

		// 当 steve 在走路或跑步时，手臂和脚会摆动，以实现走路、跑步动画
        if(camera->getPerspectiveMode() != 3) steveLeftArm->rotation[0] += steve->roateDir[2] * (isShiftPressed ? 15 : 9);
		if(steveLeftArm->rotation[0] >= 45 || steveLeftArm->rotation[0] <= -45) steve->roateDir[2] = -steve->roateDir[2];
		if(camera->getPerspectiveMode() != 3 && steve->waving == false) steveRightArm->rotation[0] += steve->roateDir[3] * (isShiftPressed ? 15 : 9);
		if(steveRightArm->rotation[0] >= 45 || steveRightArm->rotation[0] <= -45) steve->roateDir[3] = -steve->roateDir[3];
		steveLeftLeg->rotation[0] += steve->roateDir[4] * (isShiftPressed ? 15 : 9);
		if(steveLeftLeg->rotation[0] >= 45 ||steveLeftLeg->rotation[0] <= -45) steve->roateDir[4] = -steve->roateDir[4];
		steveRightLeg->rotation[0] += steve->roateDir[5] * (isShiftPressed ? 15 : 9);
		if(steveRightLeg->rotation[0] >= 45 || steveRightLeg->rotation[0] <= -45) steve->roateDir[5] = -steve->roateDir[5];
		
		// 如果按下 shift，则从走路切换为跑步，steve 的移动、手臂和脚的摆动速度均会加快
		float currentSpeed = steve->speed;
		if (isShiftPressed) {
			currentSpeed *= 2;
		}

		// 根据按键实现 steve 的前后左右移动
		if(isWPressed){
			float Theta = glm::radians(steveBody->rotation[1]);
			steve->stevePos += glm::vec3(currentSpeed * sin(Theta), 0.0, currentSpeed * cos(Theta));
		}
		else if(isSPressed){
			float Theta = glm::radians(steveBody->rotation[1]);
			steve->stevePos -= glm::vec3(currentSpeed * sin(Theta), 0.0, currentSpeed * cos(Theta));
		}
		else if(isAPressed){
			float Theta = glm::radians(steveBody->rotation[1] + 90);
			steve->stevePos += glm::vec3(currentSpeed * sin(Theta), 0.0, currentSpeed * cos(Theta));
		}
		else if(isDPressed){
			float Theta = glm::radians(steveBody->rotation[1] + 90);
			steve->stevePos -= glm::vec3(currentSpeed * sin(Theta), 0.0, currentSpeed * cos(Theta));
		}

		// 如果跑步过程中按下空格，则触发 steve 跳跃
		if(isSpacePressed){
			if (!steve->jumping) {
				steve->jumping = true;
				steve->jumpingDirection = 1;
			}
		}
    }
}

// 鼠标按键回调函数
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
			// 触发 steve 的挥手动画
			if(steve->waving == false){
				steve->waving = true;
				steve->wavingState = 0;
				if(camera->getPerspectiveMode() != 3) steveRightArm->rotation = glm::vec3(0.0, 0.0, 0.0);
			}

			// 计算 steve 和 creeper 之间的一些变量
			float distance = sqrt(pow((steve->stevePos.x - creeper->creeperPos.x), 2) + pow((steve->stevePos.z - creeper->creeperPos.z), 2));
			glm::vec3 dir = glm::vec3(creeper->creeperPos.x - steve->stevePos.x, 0.0, creeper->creeperPos.z - steve->stevePos.z);
			glm::vec3 xdir = glm::vec3(steve->steveDir.x, 0.0, steve->steveDir.z);
			glm::vec3 ydir = glm::vec3(dir.x, steve->steveDir.y, dir.z);
			float x_angle = angleBetweenVectors(dir, xdir);
			float y_angle = angleBetweenVectors(dir, ydir);
			float y_upper_bound = -glm::degrees(glm::atan(0.0332 / distance));
			float y_down_bound = -glm::degrees(glm::atan((0.5908 / distance) + (0.0332 / distance)));
			float x_bound = glm::degrees(glm::atan(0.175 / distance));

			// 触发 steve 攻击 creeper 的动画
			if(creeper->attacked == false && distance <= 2.0 && y_angle <= y_upper_bound && y_angle >= y_down_bound && x_angle <= x_bound){
				creeper->attacked = true;
				creeper->jumpingDirection = 1;
				creeper->attackedDir = glm::vec3(steve->steveDir.x, 0.0, steve->steveDir.z);
			}
        }
    }
}

// 滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// 当时全局视角或自由视角时，调用相机的鼠标响应函数
    if (camera->getPerspectiveMode() == 1 || camera->getPerspectiveMode() == 5) {
        camera->mouseScroll(xoffset, yoffset);
    }
}

// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // 计算鼠标的水平和竖直相对移动量
    float deltaX = xpos - lastX; // 水平方向的变化
    float deltaY = ypos - lastY; // 竖直方向的变化

    // 获取当前相机模式
    int cameraMode = camera->getPerspectiveMode();

    // 如果是全局视角模式 (1) 或自由视角模式 (5)
    if(cameraMode == 1 || cameraMode == 5){
        // 调用相机的 mouse 函数，更新相机视角
        camera->mouse(deltaX, deltaY);
    }
    // 如果是第三人称视角1 (2) 或第一人称视角 (3)
    else if(cameraMode == 2 || cameraMode == 3){
        // 根据鼠标的水平移动，调整人物的旋转角度（左右旋转）
        if (deltaX > 0) {
            steveBody->rotation[1] -= abs(deltaX) * (cameraMode == 2 ? 0.6f : 0.3f);  // 鼠标向右滑动
        }
        else if (deltaX < 0) {
            steveBody->rotation[1] += abs(deltaX) * (cameraMode == 2 ? 0.6f : 0.3f);  // 鼠标向左滑动
        }

        // 根据鼠标的竖直移动，调整人物头部的俯仰角度（上下旋转）
        if (deltaY > 0) {
            steveHead->rotation[0] = glm::min(steveHead->rotation[0] + abs(deltaY) * (cameraMode == 2 ? 0.05f : 0.1f), (cameraMode == 2 ? 89.9f : 50.0f));
        }
        else if (deltaY < 0) {
            steveHead->rotation[0] = glm::max(steveHead->rotation[0] - abs(deltaY) * (cameraMode == 2 ? 0.05f : 0.1f), -89.9f);
        }
    }
    // 如果是第三人称视角2 (4)
    else if(cameraMode == 4){
        // 根据鼠标的水平移动，调整人物身体的旋转角度（左右旋转）
        if (deltaX > 0) {
            steveBody->rotation[1] += abs(deltaX) * 0.7f;  // 鼠标向右滑动
        }
        else if (deltaX < 0) {
            steveBody->rotation[1] -= abs(deltaX) * 0.7f;  // 鼠标向左滑动
        }

        // 根据鼠标的竖直移动，调整人物头部的俯仰角度（上下旋转）
        if (deltaY > 0) {
            steveHead->rotation[0] = glm::min(steveHead->rotation[0] + abs(deltaY) * 0.1f, 89.9f);
        }
        else if (deltaY < 0) {
            steveHead->rotation[0] = glm::max(steveHead->rotation[0] - abs(deltaY) * 0.1f, -89.9f);
        }
    }

    // 更新鼠标位置
    lastX = xpos;
    lastY = ypos;
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

// 实时更新 steve 和 creeper 的互动
void updateInteraction(){
	steveMove();
	steve->updateJumpAnimation();
	steve->updateWavingAnimation();
	creeper->updateCreeperAttacked();
	creeper->updateCreeperPos();
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
	window = glfwCreateWindow(600, 600, "期末大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouse_callback);  // 设置鼠标位置回调函数
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// 隐藏鼠标
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		updateInteraction();	// 实时更新互动
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cleanData();

	return 0;
}