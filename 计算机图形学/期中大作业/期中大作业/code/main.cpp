/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * - 3) 绘制‘J’、‘Z’等形状的方块
 * - 4) 随机生成方块并赋上不同的颜色
 * - 5) 方块的自动向下移动
 * - 6) 方块之间、方块与边界之间的碰撞检测
 * - 7) 棋盘格中每一行填充满之后自动消除
 * - 8) 键盘'S'键控制游戏暂停
 * - 9) 暂停标志的绘制
 * - 10) 键盘空格键加速方块下移
 * - 11) 右上角显示计分器
 * - 12) 左上角显示计时器
 * - 13) 当窗口被占满而不能再出现新的方块时，游戏结束
 * - 14) 游戏失败标志的绘制
 * - 15) 键盘'R'键控制游戏重启
 *
 *
 */

#include "include/Angel.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <random>
const int CIRCLE_NUM_POINTS = 100;
const int TRIANGLE_NUM_POINTS  = 3;
const int RECTANGLE_NUM_POINTS = 4;

int last_move_time = glfwGetTime();	// 记录上次方块移动的时间
int stopTime = 0;	// 用于记录暂停时间
int last_cal_time = glfwGetTime();	// 记录上次计时的时间
int timerSeconds = 0; // 计时器的秒数
int rotation = 0;		// 控制当前窗口中的方块旋转
glm::vec2 tile[4];			// 表示当前窗口中的方块
bool gameover = false;	// 游戏结束控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;
int nowBlock;	// 当前方块形状
int nowBlockColor;	// 当前方块颜色
int score = 0;	// 获得的分数
int paused = 0;	// 游戏暂停标志
double boardOffsetY = -25;

// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num =  (board_width + 1) + (board_height + 1);

// 一个二维数组表示所有可能出现的方块和方向。
glm::vec2 allshape[7][4][4] ={
							{{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1)},	//   "L"
							{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1)},   //
							{glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},   //
							{glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}},

							{{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(1,-1)},	//   "J"
							{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1)},   //
							{glm::vec2(-1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},   //
							{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)}},

							{{glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, 0)},	//   "T"
							{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0)},   //
							{glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1)},   //
							{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, 1)}},

							{{glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, -1)},	//   "Z"
							{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1)},   //
							{glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, -1)},	//
							{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1)}},

							{{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, 0)},	//   "S"
							{glm::vec2(1, -1), glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1)},   //
							{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(1, 0)},	//
							{glm::vec2(1, -1), glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1)}},

							{{glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0)},	//   "I"
							{glm::vec2(0, -2), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)},   //
							{glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0)},	//   
							{glm::vec2(0, -2), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)}},

							{{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(-1, 0), glm::vec2(0, 0)},	//   "O"
							{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(-1, 0), glm::vec2(0, 0)},   //
							{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(-1, 0), glm::vec2(0, 0)},	//   
							{glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(-1, 0), glm::vec2(0, 0)}}
};

// 绘制窗口的颜色变量
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);	// 白色
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);	// 黑色
glm::vec4 gray    = glm::vec4(0.5, 0.5, 0.5, 1.0);   // 灰色
glm::vec4 red    = glm::vec4(1.0, 0.0, 0.0, 1.0);   // 红色
glm::vec4 green  = glm::vec4(0.0, 1.0, 0.0, 1.0);   // 绿色
glm::vec4 blue   = glm::vec4(0.0, 0.0, 1.0, 1.0);   // 蓝色
glm::vec4 yellow = glm::vec4(1.0, 1.0, 0.0, 1.0);   // 黄色
glm::vec4 cyan   = glm::vec4(0.0, 1.0, 1.0, 1.0);   // 青色
glm::vec4 magenta= glm::vec4(1.0, 0.0, 1.0, 1.0);   // 品红色
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);   // 橙色
glm::vec4 purple  = glm::vec4(0.5, 0.0, 0.5, 1.0);   // 紫色
glm::vec4 brown   = glm::vec4(0.6, 0.3, 0.2, 1.0);   // 棕色

// 方块颜色
glm::vec4 blockColor[9] = {
	glm::vec4(1.0, 0.5, 0.0, 1.0),	// 橙色
	glm::vec4(0.5, 0.0, 0.0, 1.0),  // 红色
	glm::vec4(0.5, 0.5, 0.0, 1.0),  // 黄色
	glm::vec4(0.0, 0.0, 0.5, 1.0),  // 蓝色
	glm::vec4(0.0, 0.5, 0.0, 1.0),  // 绿色
	glm::vec4(0.0, 0.5, 0.5, 1.0),  // 青色
	glm::vec4(0.5, 0.0, 0.5, 1.0), 	// 紫色
	glm::vec4(0.6, 0.3, 0.2, 1.0),  // 棕色
	glm::vec4(1.0, 0.0, 1.0, 1.0)	// 品红色
};

// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

std::string vshader = "shaders/vshader.glsl";
std::string fshader = "shaders/fshader.glsl";
GLuint program;
GLuint vao[30];
GLuint vbo[30];

int scoreLineNums;	// 计分器数字的段数
// 存储计分器的顶点数据和颜色
glm::vec4 scorePoints[42];
glm::vec4 scoreColor[42];

int timeLineNums;	// 计时器的段数	
// 存储计时器的顶点数据和颜色
glm::vec4 timePoints[56];
glm::vec4 timeColor[56];

int loseLineNums;	// LOSE的段数

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos)
{
    // 检查格子坐标是否在有效范围内，且该位置未被填充
    if((cellpos.x >= 0) && (cellpos.x < board_width) && 
       (cellpos.y >= 0) && (cellpos.y < board_height) && 
       (board[(int)cellpos.x][(int)cellpos.y] == false))
        return true; // 位置有效
    else
        return false; // 位置无效
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
    // 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
    for (int i = 0; i < 6; i++)
        board_colours[(int)(6 * (board_width * pos.y + pos.x) + i)] = colour; // 更新颜色数组

    glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour}; // 新颜色数组

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

    // 计算偏移量，在适当的位置赋上颜色
    int offset = 6 * sizeof(glm::vec4) * (int)(board_width * pos.y + pos.x);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours); // 更新颜色数据
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑缓冲区
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width) + boardOffsetY, .4, 1);
		glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width) + boardOffsetY, .4, 1);
		glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width) + boardOffsetY, .4, 1);
		glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width) + boardOffsetY, .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints);
	}
	glBindVertexArray(0);
	
}

// 设置当前方块为下一个即将出现的方块。
// 在游戏开始的时候调用来创建一个初始的方块，
// 同时判断是否有足够的空间来生成新的方块，否则游戏结束
void newtile() 
{
    // 将新方块放于棋盘格的最上行中间位置
    tilepos = glm::vec2(5, 19);

    // 设置随机数生成的范围
    int minrandom = 0, maxrandom = 100;
    std::random_device seed; // 硬件生成随机数种子
    std::ranlux48 engine(seed()); // 利用种子生成随机数引擎
    std::uniform_int_distribution<> distrib(minrandom, maxrandom); // 设置随机数范围，并为均匀分布

    // 随机选择当前方块类型、颜色和形状
    nowBlock = distrib(engine) % 7;
    nowBlockColor = distrib(engine) % 9;
    int nowBlockshape = distrib(engine) % 4;

    // 将随机生成的方块形状赋值给当前方块
    for (int i = 0; i < 4; i++)
    {
        tile[i] = allshape[nowBlock][nowBlockshape][i];
    }

	// 确保方块不会超出网格
	for (int i = 0; i < 4; ++i) {
        GLfloat x = tilepos.x + tile[i].x;
        GLfloat y = tilepos.y + tile[i].y;
        if (y >= board_height) {
            tilepos.y -= 1;
			break;
        }
    }

    // 检查是否有足够的空间来生成新的方块，如果没有，则游戏结束
    for (int i = 0; i < 4; ++i) {
        GLfloat x = tilepos.x + tile[i].x;
        GLfloat y = tilepos.y + tile[i].y;
        if (checkvalid(glm::vec2(x, y)) == false) {
            gameover = true; // 标记游戏结束
        }
    }
    // 如果游戏结束，输出提示信息
    if (gameover == true) 
        std::cout << "Game over! Please press R to restart or press Q to quit the game." << std::endl;

    updatetile(); // 更新当前方块状态

    // 给新方块赋上颜色
    glm::vec4 newcolours[24];
    for (int i = 0; i < 24; i++)
        newcolours[i] = blockColor[nowBlockColor]; // 使用随机选择的颜色

    // 更新顶点缓冲区中的颜色数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // 更新颜色数据
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑缓冲区

    glBindVertexArray(0); // 解绑当前的 VAO
}


// 绘制一个数字，数字由七个段组成，位置和缩放因子由参数指定
void drawDigit(int &digit, glm::vec4 &position, glm::vec4 vertices[], int &lineNums, float scale) {
    // 每个数字的段表示，true表示该段被激活
    bool segments[7] = { false };

    // 根据数字确定哪些段应该被激活
    switch (digit) {
        case 0: segments[0] = segments[1] = segments[2] = segments[3] = segments[4] = segments[5] = true; break; // 0
        case 1: segments[1] = segments[2] = true; break; // 1
        case 2: segments[0] = segments[1] = segments[3] = segments[4] = segments[6] = true; break; // 2
        case 3: segments[0] = segments[1] = segments[2] = segments[3] = segments[6] = true; break; // 3
        case 4: segments[1] = segments[2] = segments[5] = segments[6] = true; break; // 4
        case 5: segments[0] = segments[2] = segments[3] = segments[5] = segments[6] = true; break; // 5
        case 6: segments[0] = segments[2] = segments[3] = segments[4] = segments[5] = segments[6] = true; break; // 6
        case 7: segments[0] = segments[1] = segments[2] = true; break; // 7
        case 8: segments[0] = segments[1] = segments[2] = segments[3] = segments[4] = segments[5] = segments[6] = true; break; // 8
        case 9: segments[0] = segments[1] = segments[2] = segments[5] = segments[6] = true; break; // 9
    }

    // 定义每个段的坐标
    glm::vec2 segment[6] = {
        {-0.3, 0.5}, // 上段
        {0.3, 0.5}, // 右上段
        {0.3, 0.0}, // 中段
        {0.3, -0.5}, // 右下段
        {-0.3, -0.5}, // 下段
        {-0.3, 0.0}, // 左中段
    }; 

    // 按照缩放因子调整段的坐标
    for(int i = 0; i < 6; ++i) {
        segment[i].x *= scale;
        segment[i].y *= scale;
    }

    // 根据激活的段绘制线段
    for(int i = 0; i < 6; ++i) {
        if (segments[i]) {
            // 添加线段的两个顶点
            vertices[lineNums * 2] = glm::vec4(position.x + segment[i].x, position.y + segment[i].y, position.z, 1);
            vertices[lineNums * 2 + 1] = glm::vec4(position.x + segment[(i + 1) % 6].x, position.y + segment[(i + 1) % 6].y, position.z, 1);
            ++lineNums; // 更新线段数量
        }
    }   

    // 特殊处理中间段的绘制
    if (segments[6]) {
        vertices[lineNums * 2] = glm::vec4(position.x + segment[2].x, position.y + segment[2].y, position.z, 1);
        vertices[lineNums * 2 + 1] = glm::vec4(position.x + segment[5].x, position.y + segment[5].y, position.z, 1);
        ++lineNums;
    }
}

// 更新得分显示
void updateScore() {
    scoreLineNums = 0; // 重置线段计数

    int tempScore = score % 1000; // 获取得分的最后三位
    int digits[3] = {0}; // 假设最多三位数
    for (int i = 0; i < 3; ++i) {
        digits[i] = tempScore % 10; // 提取每一位数字
        tempScore /= 10;
        if(tempScore == 0) break; // 如果得分已经处理完毕，则提前结束
    }
    
    // 设置数字的位置在右上角
    glm::vec4 position = glm::vec4((board_width + 1) * tile_width, (board_height + 1) * tile_width, 0.5, 1);
    
    // 绘制每一位数字
    for (int i = 0; i < 3; ++i) {
        drawDigit(digits[i], position, scorePoints, scoreLineNums, 30); // 绘制数字
        position.x -= 25; // 调整位置，绘制下一个数字
    }

    // 设置得分的颜色为白色
    for(int i = 0; i < scoreLineNums * 2; ++i) {
        scoreColor[i] = white;
    }

    // 将得分的顶点数据上传到GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, scoreLineNums * 2 * sizeof(glm::vec4), scorePoints);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 上传得分颜色数据到GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, scoreLineNums * 2 * sizeof(glm::vec4), scoreColor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 更新计时器
void updateTime() {
	int current_time = glfwGetTime();	// 获取当前时间
    // 每过一秒且游戏未结束且未暂停，增加计时器秒数
    if (current_time - last_cal_time >= 1 && gameover == false && !paused){
		last_cal_time = current_time; // 更新最后计算时间
		++timerSeconds; // 增加秒数
	}

	timeLineNums = 0; // 重置计时器段数
	int timer[4] = {0}; // 假设最多四位数字（MM:SS）
	int min = timerSeconds / 60, sec = timerSeconds % 60; // 计算分钟和秒数
	if(min == 60) min = 0; // 如果分钟达到60，重置为0
	// 提取秒数的每一位
	for (int i = 0; i < 2; ++i) {
        timer[i] = sec % 10; // 提取当前秒的最后一位
        sec /= 10; // 右移一位
		if(sec == 0) break; // 如果秒数为0，停止提取
    }
	// 提取分钟的每一位
	for (int i = 2; i < 4; ++i) {
        timer[i] = min % 10; // 提取当前分钟的最后一位
        min /= 10; // 右移一位
		if(min == 0) break; // 如果分钟为0，停止提取
    }

	// 设置计时器显示位置
	glm::vec4 position = glm::vec4(tile_width, (board_height+1) * tile_width, 0.5, 1); // 左上角位置
    // 绘制每个数字
    for (int i = 3; i >= 0; --i) {
        drawDigit(timer[i], position, timePoints, timeLineNums, 30); // 绘制当前数字
        position.x += 25; // 调整位置，绘制下一个数字
		if(i == 2) position.x += 10; // 在分钟和秒之间增加间隔
    }
	
	// 设置所有数字的颜色为白色
	for(int i = 0; i < timeLineNums * 2; ++i){
		timeColor[i] = white;
	}

	// 更新VBO，上传时间顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, timeLineNums * 2 * sizeof(glm::vec4), timePoints);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO

	// 更新VBO，上传时间颜色数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, timeLineNums * 2 * sizeof(glm::vec4), timeColor);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO
}

// 绘制圆
void drawCircle(glm::vec4 center, float radius, glm::vec4 vertices[], glm::vec4 colors[], int startidx, int numSegments, glm::vec4 color, float scale) {
    float angleIncrement = 2.0f * M_PI / numSegments; // 计算每个段的角度增量
    for (int i = 0; i < numSegments; ++i) {
        float theta = i * angleIncrement; // 当前段的角度
        glm::vec2 point = glm::vec2((center.x + radius * cos(theta) * scale), (center.y + radius * sin(theta) * scale));
        vertices[startidx + i] = glm::vec4(point.x, point.y, center.z, 1);
		colors[startidx + i] = color;
    }
}

// 绘制三角形
void drawTriangle(glm::vec4 center, glm::vec4 vertices[], glm::vec4 colors[], glm::vec4 color, float scale) {
    // 设置三角形的三个顶点
    vertices[0] = center + glm::vec4(0.15f * scale, 0.0f * scale, 0.0f, 1.0f); // 第一个顶点
    vertices[1] = center + glm::vec4(-0.2f * scale, 0.15f * scale, 0.0f, 1.0f); // 第二个顶点
    vertices[2] = center + glm::vec4(-0.2f * scale, -0.15f * scale, 0.0f, 1.0f); // 第三个顶点

    // 设置三角形的颜色
    colors[0] = color; // 第一个顶点的颜色
    colors[1] = color; // 第二个顶点的颜色
    colors[2] = color; // 第三个顶点的颜色
}

// 绘制长方形
void drawRectangle(glm::vec4 center, glm::vec4 vertices[], glm::vec4 colors[], glm::vec4 color, float scale) {
    // 设置长方形的四个顶点
    vertices[0] = center + glm::vec4(0.8f * scale, 0.35f * scale, 0.0f, 1.0f); // 右上角
    vertices[1] = center + glm::vec4(-0.8f * scale, 0.35f * scale, 0.0f, 1.0f); // 左上角
    vertices[2] = center + glm::vec4(-0.8f * scale, -0.35f * scale, 0.0f, 1.0f); // 左下角
    vertices[3] = center + glm::vec4(0.8f * scale, -0.35f * scale, 0.0f, 1.0f); // 右下角

    // 设置长方形的颜色
    colors[0] = color; // 右上角的颜色
    colors[1] = color; // 左上角的颜色
    colors[2] = color; // 左下角的颜色
    colors[3] = color; // 右下角的颜色
}

// 定义绘制字符的函数，输入为字符、位置、顶点数组、线段数和缩放比例
void drawChar(char &ch, glm::vec4 &position, glm::vec4 vertices[], int &lineNums, float scale) {
	// 每个字符的段表示
    bool segments[7] = { false };

    // 根据字符设置对应的段
    switch (ch) {
        case 'L': segments[3] = segments[4] = segments[5] = true; break; // 'L'
        case 'O': segments[0] = segments[1] = segments[2] = segments[3] = segments[4] = segments[5] = true; break; // 'O'
        case 'S': segments[0] = segments[2] = segments[3] = segments[5] = segments[6] = true; break; // 'S'
        case 'E': segments[0] = segments[3] = segments[4] = segments[5] = segments[6] = true; break; // 'E'
    }

	// 定义段的坐标
	glm::vec2 segment[6] = {
        {-0.3, 0.4},
		{0.3, 0.4},
		{0.3, 0.0},
		{0.3, -0.4},
		{-0.3, -0.4},
		{-0.3, 0.0},
    };
	// 缩放段坐标
	for(int i = 0; i < 6; ++i) {
		segment[i].x *= scale;
		segment[i].y *= scale;
	}

	// 根据段数组绘制字符的顶点
	for(int i = 0; i < 6; ++i) {
		if (segments[i]) {
			vertices[lineNums * 2] = glm::vec4(position.x + segment[i].x, position.y + segment[i].y, position.z, 1);
			vertices[lineNums * 2 + 1] = glm::vec4(position.x + segment[(i + 1) % 6].x, position.y + segment[(i + 1) % 6].y, position.z, 1);
			++lineNums;
		}
	}
	// 特殊处理最后一个段
	if (segments[6]) {
		vertices[lineNums * 2] = glm::vec4(position.x + segment[2].x, position.y + segment[2].y, position.z, 1);
		vertices[lineNums * 2 + 1] = glm::vec4(position.x + segment[5].x, position.y + segment[5].y, position.z, 1);
		++lineNums;
	}
}

// 绘制失败标志的函数，输入为中心位置、顶点数组、颜色数组、颜色和缩放比例
void drawLoseFlag(glm::vec4 center, glm::vec4 vertices[], glm::vec4 colors[], glm::vec4 color, float scale) {
	std::string lose = "LOSE"; // 失败标志文本
	loseLineNums = 0; // 重置线段数量
	glm::vec4 tmpPosition = center; // 复制中心位置

	// 绘制前两个字符
	tmpPosition.x -= 22.5; // 调整位置
    for (int i = 1; i >= 0; --i) {
        drawChar(lose[i], tmpPosition, vertices, loseLineNums, scale);
        tmpPosition.x -= 45; // 调整位置，绘制下一个字符
    }

	// 绘制后两个字符
	tmpPosition = center; // 重置位置
	tmpPosition.x += 22.5; // 调整位置
    for (int i = 2; i < 4; ++i) {
        drawChar(lose[i], tmpPosition, vertices, loseLineNums, scale);
        tmpPosition.x += 45; // 调整位置，绘制下一个字符
    }

	// 设置颜色
	for(int i = 0; i < loseLineNums * 2; ++i) {
		colors[i] = color;
	}
}

void setupVertexArray(GLuint &vao, GLuint vbo[], glm::vec4 vertices[], glm::vec4 colors[], size_t sizes, GLuint vPosition, GLuint vColor, GLenum usage[]) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	glGenBuffers(2, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizes, vertices, usage[0]);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizes, colors, usage[1]);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}


// 游戏和OpenGL初始化
void init()
{
	// 初始化棋盘格，这里用画直线的方法绘制网格
	// 包含竖线 board_width+1 条
	// 包含横线 board_height+1 条
	// 一条线2个顶点坐标，并且每个顶点一个颜色值
	
	glm::vec4 gridpoints[board_line_num * 2];
	glm::vec4 gridcolours[board_line_num * 2];

	// 绘制网格线
	// 纵向线
	for (int i = 0; i < (board_width+1); i++)
	{
		gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width + boardOffsetY, 0, 1);
		gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width + boardOffsetY, 0, 1);
	}

	// 水平线
	for (int i = 0; i < (board_height+1); i++)
	{
		gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)) + boardOffsetY, 0, 1);
		gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)) + boardOffsetY, 0, 1);
	}

	// 将所有线赋成白色
	for (int i = 0; i < (board_line_num * 2); i++)
		gridcolours[i] = white;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	glm::vec4 boardpoints[points_num];
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
		{
			glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width) + boardOffsetY, .5, 1);
			glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width) + boardOffsetY, .5, 1);
			glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width) + boardOffsetY, .5, 1);
			glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width) + boardOffsetY, .5, 1);
			boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
			boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++)
			board[i][j] = false;

	// 计分器
	updateScore();

	// 计时器
	updateTime();
	// 绘制计时器的冒号
	glm::vec4 position = glm::vec4(tile_width, (board_height+1) * tile_width, 0.5, 1); // 左上角位置
	glm::vec4 colonpoints1[CIRCLE_NUM_POINTS]; // 存储冒号上半部分的圆形顶点
	glm::vec4 coloncolours1[CIRCLE_NUM_POINTS]; // 存储冒号上半部分的颜色
	drawCircle(glm::vec4(position.x + 43, position.y + 8, position.z, 1), 1, colonpoints1, coloncolours1, 0, CIRCLE_NUM_POINTS, white, 3); // 绘制上半部分冒号
	glm::vec4 colonpoints2[CIRCLE_NUM_POINTS]; // 存储冒号下半部分的圆形顶点
	glm::vec4 coloncolours2[CIRCLE_NUM_POINTS]; // 存储冒号下半部分的颜色
	drawCircle(glm::vec4(position.x + 43, position.y - 8, position.z, 1), 1, colonpoints2, coloncolours2, 0, CIRCLE_NUM_POINTS, white, 3); // 绘制下半部分冒号

	// 定义暂停标志的圆形和三角形的顶点和颜色数组
	glm::vec4 stopCirclePoints[CIRCLE_NUM_POINTS]; // 存储暂停圆形的顶点
	glm::vec4 stopCircleColours[CIRCLE_NUM_POINTS]; // 存储暂停圆形的颜色
	glm::vec4 stopTrianglePoints[TRIANGLE_NUM_POINTS]; // 存储暂停三角形的顶点
	glm::vec4 stopTriangleColours[TRIANGLE_NUM_POINTS]; // 存储暂停三角形的颜色

	// 绘制暂停标志的圆形
	drawCircle(glm::vec4((board_width + 2) * tile_width / 2, (board_height + 1) * tile_width / 2, 1.0, 1), // 圆心位置
				1, stopCirclePoints, stopCircleColours, 0, CIRCLE_NUM_POINTS, gray, 40);

	// 绘制暂停标志的三角形
	drawTriangle(glm::vec4((board_width + 2.5) * tile_width, (board_height + 1) * tile_width, 1.0, 1), // 三角形顶点位置
				stopTrianglePoints, stopTriangleColours, white, 300);

	// 绘制矩形和失败标志
	glm::vec4 rectanglePoints[RECTANGLE_NUM_POINTS]; // 矩形顶点
	glm::vec4 rectangleColours[RECTANGLE_NUM_POINTS]; // 矩形颜色
	glm::vec4 losePoints[56]; // 失败标志顶点
	glm::vec4 loseColours[56]; // 失败标志颜色
	drawRectangle(glm::vec4((board_width + 2) * tile_width, (board_height + 1) * tile_width, 1.0, 1), rectanglePoints, rectangleColours, gray, 250);
	drawLoseFlag(glm::vec4((board_width + 2) * tile_width / 2, (board_height + 1) * tile_width / 2, 1.0, 1), losePoints, loseColours, red, 60);

	// 载入着色器
	program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	
	glGenVertexArrays(10, &vao[0]);

	// 棋盘格顶点
	GLenum gridUsages[] = {
		GL_STATIC_DRAW,
		GL_STATIC_DRAW
	};
	setupVertexArray(vao[0], vbo, gridpoints, gridcolours, board_line_num * 2 * sizeof(glm::vec4), vPosition, vColor, gridUsages);	

	// 棋盘格每个格子
	GLenum boardUsages[] = {
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW
	};
	setupVertexArray(vao[1], vbo + 2, boardpoints, board_colours, points_num * sizeof(glm::vec4), vPosition, vColor, boardUsages);	

	// 当前方块
	GLenum blockUsages[] = {
		GL_DYNAMIC_DRAW,
		GL_DYNAMIC_DRAW
	};
	setupVertexArray(vao[2], vbo + 4, NULL, NULL, 24 * sizeof(glm::vec4), vPosition, vColor, blockUsages);	

	// 计分器
	GLenum scoreUsages[] = {
		GL_DYNAMIC_DRAW,
		GL_DYNAMIC_DRAW
	};
	setupVertexArray(vao[3], vbo + 6, scorePoints, scoreColor, scoreLineNums * 2 * sizeof(glm::vec4), vPosition, vColor, scoreUsages);

	// 计时器
	GLenum timeUsages[] = {
		GL_DYNAMIC_DRAW,
		GL_DYNAMIC_DRAW
	};
	setupVertexArray(vao[4], vbo + 8, timePoints, timeColor, timeLineNums * 2 * sizeof(glm::vec4), vPosition, vColor, timeUsages);
	// 计时器的冒号
	GLenum colonUsages[] = {
		GL_STATIC_DRAW,
		GL_STATIC_DRAW
	};
	setupVertexArray(vao[5], vbo + 10, colonpoints1, coloncolours1, CIRCLE_NUM_POINTS * sizeof(glm::vec4), vPosition, vColor, colonUsages);
	setupVertexArray(vao[6], vbo + 12, colonpoints2, coloncolours2, CIRCLE_NUM_POINTS * sizeof(glm::vec4), vPosition, vColor, colonUsages);

	// 暂停标志
	GLenum stopFlagUsages[] = {
		GL_STATIC_DRAW,
		GL_STATIC_DRAW
	};
	setupVertexArray(vao[7], vbo + 14, stopCirclePoints, stopCircleColours, CIRCLE_NUM_POINTS * sizeof(glm::vec4), vPosition, vColor, stopFlagUsages);
	setupVertexArray(vao[8], vbo + 16, stopTrianglePoints, stopTriangleColours, TRIANGLE_NUM_POINTS * sizeof(glm::vec4), vPosition, vColor, stopFlagUsages);

	// 失败标志
	GLenum loseFlagUsages[] = {
		GL_STATIC_DRAW,
		GL_STATIC_DRAW
	};
	setupVertexArray(vao[9], vbo + 18, rectanglePoints, rectangleColours, RECTANGLE_NUM_POINTS * sizeof(glm::vec4), vPosition, vColor, loseFlagUsages);
	setupVertexArray(vao[10], vbo + 20, losePoints, loseColours, loseLineNums * 2 * sizeof(glm::vec4), vPosition, vColor, loseFlagUsages);

	glBindVertexArray(0);

	glClearColor(0, 0, 0, 0);

	// 游戏初始化
	newtile();
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allshape[nowBlock][nextrotation][0]) + tilepos)
		&& checkvalid((allshape[nowBlock][nextrotation][1]) + tilepos)
		&& checkvalid((allshape[nowBlock][nextrotation][2]) + tilepos)
		&& checkvalid((allshape[nowBlock][nextrotation][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;
		for (int i = 0; i < 4; i++)
			tile[i] = allshape[nowBlock][rotation][i];

		updatetile();
	}
}

// 检查棋盘有没有被填充满的行
void checkfullrow()
{
    int seq = 0;    // 记录连续填满的行数，用于分数的额外奖励
    int tmpScore = 0;    // 记录增加的分数
    int fullrows = 0;    // 用于记录填满的行数

    // 遍历每一行
    for(int row = 0; row < board_height; ++row){
        int flag = 1; // 标记行是否填满
        for(int col = 0; col < board_width; ++col){
            if(board[col][row] != true){ // 检查该行是否有未填充的格子
                flag = 0; // 行未填满
                break;
            }
        }

        // 如果该行被填满
        if(flag == 1){    
            tmpScore += 1 + seq * 1; // 更新分数
            ++seq; // 连续填满行数增加
            ++fullrows; // 填满行数增加

            // 清空该行
            for (int col = 0; col < board_width; ++col) {
                board[col][row] = false; // 将该行的状态设置为未填充
                changecellcolour(glm::vec2(col, row), black); // 更新对应颜色为黑色
            }

            // 将上面的颜色向下移动一格
            for(int upRow = row + 1; upRow < board_height; ++upRow){
                for(int upCol = 0; upCol < board_width; ++upCol){
                    if(board[upCol][upRow] == true){ // 检查上面行的填充状态
                        board[upCol][upRow - 1] = true; // 将上方的方块下移
                        glm::vec4 color = board_colours[(int)(6 * (board_width * upRow + upCol) )];
                        changecellcolour(glm::vec2(upCol, upRow - 1), color); // 更新颜色
                        board[upCol][upRow] = false; // 将原位置清空
                        changecellcolour(glm::vec2(upCol, upRow), black); // 更新原位置颜色为黑色
                    }
                }
            }
            --row; // 检查新上升后的行
        }
        else{
            seq = 0; // 如果当前行没有填满，重置连续填满计数
        }
    }

    // 更新分数并输出信息
    if(tmpScore){
        score = (score + tmpScore) % 1000; // 更新总分数
        std::cout << "Eliminate " << fullrows << " rows, increase score by "
			 		<< tmpScore << "." << std::endl; // 输出填满行数及得分
    }
}

// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		changecellcolour(glm::vec2(x, y), blockColor[nowBlockColor]);
	}
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
    // 计算移动之后的方块的位置坐标
    glm::vec2 newtilepos[4];
    for (int i = 0; i < 4; i++)
        newtilepos[i] = tile[i] + tilepos + direction;

    // 检查移动之后的有效性
    if (checkvalid(newtilepos[0])
        && checkvalid(newtilepos[1])
        && checkvalid(newtilepos[2])
        && checkvalid(newtilepos[3]))
    {
        // 有效：移动该方块
        tilepos.x = tilepos.x + direction.x;
        tilepos.y = tilepos.y + direction.y;

        updatetile(); // 更新方块状态

        return true; // 移动成功
    }
    return false; // 移动失败
}

// 重新启动游戏
void restart()
{
    gameover = false; // 设置游戏状态为未结束
    score = 0; // 重置分数为0
    last_move_time = glfwGetTime(); // 记录当前时间为最后一次方块移动时间
    last_cal_time = glfwGetTime(); // 记录当前时间为最后一次计算时间
    timerSeconds = 0; // 重置计时器秒数为0
	paused = 0;	// 重置暂停状态
    std::cout << "Game restart! Hope you can achieve better results." << std::endl; // 提示玩家游戏已重启
    init(); // 重新初始化游戏设置和资源
}


// 游戏渲染部分，负责在屏幕上绘制各种游戏元素
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲区，准备绘制新帧

	// 设置统一变量，传递窗口大小给着色器
	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	// 绘制游戏棋盘格
	glBindVertexArray(vao[1]); // 绑定棋盘格的顶点数组对象
	glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格的三角形

	// 绘制当前方块
	glBindVertexArray(vao[2]); // 绑定当前方块的顶点数组对象
	glDrawArrays(GL_TRIANGLES, 0, 24); // 绘制8个三角形表示方块

	// 绘制棋盘格的网格线
	glBindVertexArray(vao[0]); // 绑定棋盘格线的顶点数组对象
	glDrawArrays(GL_LINES, 0, board_line_num * 2); // 绘制线段表示的网格线

	// 绘制计时器冒号
	glBindVertexArray(vao[5]); // 绑定左侧冒号的顶点数组对象
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS); // 绘制左冒号的圆形
	glBindVertexArray(vao[6]); // 绑定右侧冒号的顶点数组对象
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS); // 绘制右冒号的圆形

	// 如果游戏暂停，绘制暂停标志
	if(paused){
		glBindVertexArray(vao[7]); // 绑定暂停标志左圆的顶点数组对象
		glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS); // 绘制左圆
		glBindVertexArray(vao[8]); // 绑定暂停标志三角形的顶点数组对象
		glDrawArrays(GL_TRIANGLE_FAN, 0, TRIANGLE_NUM_POINTS); // 绘制暂停标志的三角形
	}

	// 设置线宽为3.0f用于绘制计分器
	glLineWidth(3.0f);
	glBindVertexArray(vao[3]); // 绑定计分器的顶点数组对象
	glDrawArrays(GL_LINES, 0, scoreLineNums * 2); // 绘制计分器的线段

	// 绘制计时器
	glBindVertexArray(vao[4]); // 绑定计时器的顶点数组对象
	glDrawArrays(GL_LINES, 0, timeLineNums * 2); // 绘制计时器的线段

	// 如果游戏结束，绘制失败标志
	if(gameover){
		glBindVertexArray(vao[9]); // 绑定失败标志背景矩形的顶点数组对象
		glDrawArrays(GL_TRIANGLE_FAN, 0, RECTANGLE_NUM_POINTS); // 绘制失败背景矩形
		glLineWidth(8.0f); // 设置更宽的线宽用于失败标志
		glBindVertexArray(vao[10]); // 绑定失败标志文字的顶点数组对象
		glDrawArrays(GL_LINES, 0, loseLineNums * 2); // 绘制"LOSE"字样的线段
	}

	// 将线宽还原为默认值
	glLineWidth(1.0f);
}


// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(!gameover) // 检查游戏是否未结束
    {
        switch(key)
        {	
            case GLFW_KEY_UP:	
                if (action == GLFW_PRESS || action == GLFW_REPEAT)
                {
                    if (!paused) rotate(); // 在未暂停的情况下，旋转方块
                }
                break;
            case GLFW_KEY_DOWN: 
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    if (!paused) {
                        if (!movetile(glm::vec2(0, -1))) // 尝试向下移动方块
                        {
                            settile(); // 方块到达底部，固定位置
                            newtile(); // 生成新的方块
                        }
                    }
                }
                break;
            case GLFW_KEY_SPACE: // 空格键处理
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    if (!paused) {
                        if (!movetile(glm::vec2(0, -1))) { // 尝试向下快速移动方块
                            settile();
                            newtile();
                        }
                    }
                }
                break;
            case GLFW_KEY_LEFT:  
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    if (!paused) movetile(glm::vec2(-1, 0)); // 向左移动方块
                }
                break;
            case GLFW_KEY_RIGHT: 
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    if (!paused) movetile(glm::vec2(1, 0)); // 向右移动方块
                }
                break;
            case GLFW_KEY_ESCAPE:
                if(action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS); // 退出游戏
                }
                break;
            case GLFW_KEY_Q:
                if(action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS); // 退出游戏
                }
                break;
            case GLFW_KEY_R:
                if(action == GLFW_PRESS) {
                    restart(); // 重启游戏
                }
                break;	
            case GLFW_KEY_S: // 暂停游戏
                if(action == GLFW_PRESS) {
                    paused = !paused; // 切换暂停状态
                    if(paused) {
                        std::cout << "Game Paused!" << std::endl; // 输出暂停信息
                        stopTime = glfwGetTime(); // 记录暂停时间
                    } else {
                        int stopTotalTime = glfwGetTime() - stopTime;
                        last_move_time += stopTotalTime; // 更新方块移动时间
                        last_cal_time += stopTotalTime; // 更新计算时间
                        std::cout << "Game Continue!" << std::endl; // 输出继续游戏信息
                    }
                }
                break;
        }
    }
    else {
        switch(key) {
            case GLFW_KEY_ESCAPE:
                if(action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS); // 退出游戏
                    break;
                }
            case GLFW_KEY_Q:
                if(action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS); // 退出游戏
                    break;
                }
            case GLFW_KEY_R:
                if(action == GLFW_PRESS) {
                    restart(); // 重启游戏
                    break;
                }
        }
    }
}


// 方块自动下降
void autoMove(){
    int current_time = glfwGetTime(); // 获取当前时间
	// 如果过了0.5秒且游戏未结束且未暂停
    if (current_time - last_move_time >= 0.5 && gameover == false && !paused)
    {
        last_move_time = current_time; // 更新上次移动时间
        if (!movetile(glm::vec2(0, -1))) // 尝试将方块向下移动
        {
            settile(); // 如果无法移动，设置背景颜色
            newtile(); // 启动新的方块
        }
    }
}

void displayControls() {
    std::cout << "Welcome to the Game!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "UP ARROW   : Rotate the tile" << std::endl;
    std::cout << "DOWN ARROW : Accelerate the tile down" << std::endl;
    std::cout << "SPACE      : Accelerate the tile down" << std::endl;
    std::cout << "LEFT ARROW : Move the tile left" << std::endl;
    std::cout << "RIGHT ARROW: Move the tile right" << std::endl;
    std::cout << "R          : Restart the game" << std::endl;
    std::cout << "S          : Pause/Resume the game" << std::endl;
    std::cout << "ESC        : Exit the game" << std::endl;
    std::cout << "Q          : Exit the game" << std::endl;
    std::cout << "Good luck!" << std::endl << std::endl;
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

	// 创建窗口。
	GLFWwindow* window = glfwCreateWindow(500, 900, "期中大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	
	displayControls();	// 显示游戏控制说明
	init();	// 初始化游戏设置和资源

	// 主循环，直到窗口关闭
	while (!glfwWindowShouldClose(window))
	{
		updateScore();	// 更新当前分数
		updateTime();	// 更新游戏计时
		autoMove();		// 让方块自动向下移动
		display();		// 渲染显示当前游戏状态
		
		glfwSwapBuffers(window);	// 交换前后缓冲区以显示新绘制的内容
		glfwPollEvents();   // 处理所有用户输入事件 
		
		// 仅在游戏未结束且未暂停时检查并消除填满的行
		if (gameover == false && !paused) {
			checkfullrow(); // 清空填满的行
		}
	}
    glfwTerminate();
    return 0;
}
