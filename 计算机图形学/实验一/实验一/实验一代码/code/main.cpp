#include "Angel.h"
#include <string>

const glm::vec3 WHITE(1.0, 1.0, 1.0);
const glm::vec3 BLACK(0.0, 0.0, 0.0);
const glm::vec3 RED(1.0, 0.0, 0.0);
const glm::vec3 GREEN(0.0, 1.0, 0.0);
const glm::vec3 BLUE(0.0, 0.0, 1.0); 
const glm::vec3 YELLOW(1.0, 1.0,  0.0);
const glm::vec3 CYAN(0.0, 1.0, 1.0);
const glm::vec3 PURPLE(1.0, 0.0, 1.0);
const int CIRCLE_NUM_POINTS = 100;
const int ELLIPSE_NUM_POINTS = 100;
const int TRIANGLE_NUM_POINTS  = 3;
const int SQUARE_NUM  = 6;
const int SQUARE_NUM_POINTS  = 4 * SQUARE_NUM;
const float PI = 3.1415926f;
const glm::vec2 SCALE(0.6, 0.6);

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 根据角度生成颜色
float generateAngleColor(double angle)
{
	return 1.0 / (2 * M_PI) * angle;
}

// 获得三角形的每个角度
double getTriangleAngle(int point)
{
	return 2 * M_PI / 3 * point;
}

// 获得正方形的每个角度
double getSquareAngle(int point)
{
	return M_PI / 4 + (M_PI / 2 * point);
}

// 计算给定角度在椭圆/圆上的顶点坐标
glm::vec2 getEllipseVertex(glm::vec2 center, double scale, double verticalScale, double angle)
{
    // 根据给定角度创建一个单位向量，sin(angle) 是 x 坐标，cos(angle) 是 y 坐标
    glm::vec2 vertex(sin(angle), cos(angle));

    // 将顶点放大至指定的椭圆半径（scale 表示水平半径）
    vertex *= scale;

    // 单独缩放 y 坐标，以适应垂直方向上的比例（verticalScale 表示垂直半径）
    vertex.y *= verticalScale;

    // 将顶点平移到椭圆的中心位置
    vertex += center;

    // 返回计算后的顶点坐标
    return vertex;
}

// 生成椭圆或圆的顶点和颜色信息
void generateEllipsePoints(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, int numPoints,
                           glm::vec2 center, double scale, double verticalScale)
{
    // 每个点之间的角度增量，2 * PI 是圆周角，除以顶点数量以均匀分布点
    double angleIncrement = (2 * M_PI) / numPoints;

    // 初始化角度，从 π/2 开始，意味着第一个点在椭圆的顶部
    double currentAngle = M_PI / 2;

    // 遍历生成所有的顶点
    for (int i = startVertexIndex; i < startVertexIndex + numPoints; ++i) {
        // 使用 getEllipseVertex 函数计算当前角度对应的顶点
        vertices[i] = getEllipseVertex(center, scale, verticalScale, currentAngle);

        // 如果是圆（verticalScale == 1.0），则根据角度生成颜色，颜色只在 R 通道
        if (verticalScale == 1.0) {
            colors[i] = glm::vec3(generateAngleColor(currentAngle), 0.0, 0.0);
        }
        // 如果是椭圆，则将颜色固定为 RED
        else {
            colors[i] = RED;
        }

        // 更新角度，准备计算下一个顶点
        currentAngle += angleIncrement;
    }
}

// 获得三角形的每个顶点
void generateTrianglePoints(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex)
{
	glm::vec2 scale(0.25, 0.25);
	glm::vec2 center(0.0, 0.70);

	for (int i = 0; i < 3; ++i) {
		double currentAngle = getTriangleAngle(i);
		vertices[startVertexIndex + i] = glm::vec2(sin(currentAngle), cos(currentAngle)) * scale + center;
	}

	colors[startVertexIndex] = RED;
	colors[startVertexIndex + 1] = GREEN;
	colors[startVertexIndex + 2] = BLUE;
}

// 获得正方形的每个顶点
void generateSquarePoints(glm::vec2 vertices[], glm::vec3 colors[], int squareNumber, int startVertexIndex)
{
	glm::vec2 scale(0.90, 0.90);
	double scaleDecrease = 0.15;
	glm::vec2 center(0.0, -0.25);
	int vertexIndex = startVertexIndex;

	for (int i = 0; i < squareNumber; ++i) {
		glm::vec3 currentColor;
		currentColor = (i % 2) ? BLACK : WHITE;
		for (int j = 0; j < 4; ++j) {
			double currentAngle = getSquareAngle(j);
			vertices[vertexIndex] = glm::vec2(sin(currentAngle), cos(currentAngle)) * scale + center;
			colors[vertexIndex] = currentColor;
			vertexIndex++;
		}
		scale -= scaleDecrease;
	}
}

// 生成第一种脸的顶点坐标，顶点颜色固定为黄色
void generateFace1Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float R = 0.8f;

	for(int i = 0; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(R*cos(2*PI*i/n),R*sin(2*PI*i/n)))*SCALE + CENTER;
		colors[vertexIndex] = YELLOW;
		++vertexIndex;
	}
}

// 生成第二种脸的顶点坐标，顶点颜色为随机色彩
void generateFace2Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 CENTER){
	const glm::vec3 color_sequence[8] = {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, CYAN, PURPLE};
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float R = 0.8f;

	// 在圆中心加一个顶点，确保颜色是从中心散发出去
	vertices[vertexIndex] = glm::vec2(0.0, 0.0) * SCALE + CENTER;
	colors[vertexIndex] = WHITE;
	++vertexIndex;

	for(int i = 0; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(R*cos(2*PI*i/n),R*sin(2*PI*i/n)))*SCALE + CENTER;
		colors[vertexIndex] = color_sequence[rand()%8];
		++vertexIndex;
	}
}

// 生成眼睛的顶点坐标和颜色信息
void generateEyePoints(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float a = 0.4f, b = 0.3f;
	glm::vec2 scale(0.60, 0.60);

	// 大的半椭圆
	for(int i = 0; i < n / 2; ++i){
		vertices[vertexIndex] = (glm::vec2(a*cos(2*PI*i/n),b*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = WHITE;
		++vertexIndex;
	}

	// 小的半椭圆
	for(int i = n / 2; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2((a/2)*cos(2*PI*i/n) - (a/2), (b/2)*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = WHITE;
		++vertexIndex;
	}

	// 小的半椭圆
	for(int i = n / 2; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2((a/2)*cos(2*PI*i/n) + (a/2), (b/2)*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = WHITE;
		++vertexIndex;
	}
}

// 生成眼珠的顶点坐标和颜色信息
void generateEyeballPoints(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float R = 0.15f;
	glm::vec2 scale(0.60, 0.60);

	for(int i = 0; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(R*cos(2*PI*i/n) - 0.2, R*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = BLACK;
		++vertexIndex;
	}
}

// 生成第一种嘴的顶点坐标和颜色信息
void generateMouth1Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float a = 0.4f, b = 0.3f; 
	glm::vec2 scale(0.80, 0.80);

	for(int i = 0; i < n / 2; ++i){
		vertices[vertexIndex] = (glm::vec2(0.4 - 0.8 * i / (n / 2), 0)*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = WHITE;
		++vertexIndex;
	}

	// 半椭圆
	for(int i = n / 2; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(a*cos(2*PI*i/n), b*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = WHITE;
		++vertexIndex;
	}
}

// 生成第一种嘴描边的顶点坐标和颜色信息
void generateLine1Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	float a = 0.4f, b = 0.3f; 
	glm::vec2 scale(0.80, 0.80);

	for(int i = 0; i < n / 2; ++i){
		vertices[vertexIndex] = (glm::vec2(0.4 - 0.8 * i / (n / 2), 0)*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = BLACK;
		++vertexIndex;
	}

	for(int i = n / 2; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(a*cos(2*PI*i/n), b*sin(2*PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = BLACK;
		++vertexIndex;
	}
}

// 生成第一种嘴中牙缝的顶点坐标和颜色信息
void generateLine2Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS;
	int vertexIndex = startVertexIndex;
	glm::vec2 scale(0.80, 0.80);

	vertices[vertexIndex] = (glm::vec2(0.0, 0.0)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
	vertices[vertexIndex] = (glm::vec2(0.0, -0.3)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
	vertices[vertexIndex] = (glm::vec2(0.2, 0.0)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
	vertices[vertexIndex] = (glm::vec2(0.2, -0.26)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
	vertices[vertexIndex] = (glm::vec2(-0.2, 0.0)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
	vertices[vertexIndex] = (glm::vec2(-0.2, -0.26)*scale + center)*SCALE + CENTER;
	colors[vertexIndex++] = BLACK;
}

// 生成第二种嘴的顶点坐标和颜色信息
void generateMouth2Points(glm::vec2 vertices[], glm::vec3 colors[], int startVertexIndex, glm::vec2 center, glm::vec2 CENTER){
	int n = CIRCLE_NUM_POINTS / 2;
	int vertexIndex = startVertexIndex;
	float R = 0.4f;
	glm::vec2 scale(0.80, 0.80);

	for(int i = 0; i < n; ++i){
		vertices[vertexIndex] = (glm::vec2(R*cos(PI*i/n), -R*sin(PI*i/n))*scale + center)*SCALE + CENTER;
		colors[vertexIndex] = BLACK;
		++vertexIndex;
	}
}

GLuint vao[16], program;

// 初始化顶点坐标和颜色信息
void transferringData(GLuint &vao, GLuint vbo[2], glm::vec2 vertices[], glm::vec3 colors[], const int lenth){
	glGenVertexArrays(1, &vao);		
	glBindVertexArray(vao);			

	glGenBuffers(1, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * lenth, vertices, GL_STATIC_DRAW);
	GLuint location  = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec2),
		BUFFER_OFFSET(0));

	glGenBuffers(1, &vbo[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * lenth, colors, GL_STATIC_DRAW);
	GLuint cLocation  = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(
		cLocation,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		BUFFER_OFFSET(0));
}

// // 用于生成参考图片类似的图
// void init()
// {

// 	// 定义三角形的点
// 	glm::vec2 triangle_vertices[TRIANGLE_NUM_POINTS];
// 	glm::vec3 triangle_colors[TRIANGLE_NUM_POINTS];
// 	// 定义矩形的点
// 	glm::vec2 square_vertices[SQUARE_NUM_POINTS];
// 	glm::vec3 square_colors[SQUARE_NUM_POINTS];

// 	// @TODO: 生成圆形和椭圆上的点和颜色

// 	// 定义圆形的点
// 	glm::vec2 circle_vertices[CIRCLE_NUM_POINTS];
// 	glm::vec3 circle_colors[CIRCLE_NUM_POINTS];

// 	// 定义椭圆的点
// 	glm::vec2 ellipse_vertices[CIRCLE_NUM_POINTS];
// 	glm::vec3 ellipse_colors[CIRCLE_NUM_POINTS];

// 	// 调用生成形状顶点位置的函数
// 	generateTrianglePoints(triangle_vertices, triangle_colors, 0);
// 	generateSquarePoints(square_vertices, square_colors, SQUARE_NUM, 0);
// 	generateEllipsePoints(circle_vertices, circle_colors, 0, CIRCLE_NUM_POINTS, glm::vec2{0.6,0.75}, 0.2, 1);
// 	generateEllipsePoints(ellipse_vertices, ellipse_colors, 0, CIRCLE_NUM_POINTS, glm::vec2{-0.6,0.75}, 0.2, 0.6);

// 	// 读取着色器并使用
// 	std::string vshader, fshader;
// 	vshader = "shaders/vshader.glsl";
// 	fshader = "shaders/fshader.glsl";
// 	program = InitShader(vshader.c_str(), fshader.c_str());
// 	glUseProgram(program);

// 	// 创建顶点缓存对象，vbo[2]是因为我们将要使用两个缓存对象
// 	GLuint vbo[2];

// 	// 初始化三角形的数据
// 	transferringData(vao[0], vbo, triangle_vertices, triangle_colors, TRIANGLE_NUM_POINTS);

// 	// 初始化正方形的数据
// 	transferringData(vao[1], vbo, square_vertices, square_colors, SQUARE_NUM_POINTS);

// 	// 初始化圆形的数据
// 	transferringData(vao[2], vbo, circle_vertices, circle_colors, CIRCLE_NUM_POINTS);

// 	// 初始化椭圆的数据
// 	transferringData(vao[3], vbo, ellipse_vertices, ellipse_colors, CIRCLE_NUM_POINTS);
// }


// // 用于生成参考图片类似的图
// void display(void)
// {

// 	glClear(GL_COLOR_BUFFER_BIT);

// 	glUseProgram(program);

// 	glBindVertexArray(vao[0]);
// 	glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_POINTS);
	
// 	glBindVertexArray(vao[1]);
// 	for (int i = 0; i  < SQUARE_NUM; ++i) {
// 		glDrawArrays(GL_TRIANGLE_FAN, (i  * 4), 4);
// 	}

// 	// @TODO: 绘制圆
// 	glBindVertexArray(vao[2]);
// 	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);

// 	// @TODO: 绘制椭圆
// 	glBindVertexArray(vao[3]);
// 	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	
// 	glFlush();
// }

// 用于生成自己发挥想象绘制的图
void init()
{

	// 定义第一个表情的顶点数组和颜色数组
	glm::vec2 face_vertices1[CIRCLE_NUM_POINTS];
	glm::vec3 face_colors1[CIRCLE_NUM_POINTS];
	glm::vec2 lefteye_vertices1[CIRCLE_NUM_POINTS/2*3];
	glm::vec3 lefteye_colors1[CIRCLE_NUM_POINTS/2*3];
	glm::vec2 righteye_vertices1[CIRCLE_NUM_POINTS/2*3];
	glm::vec3 righteye_colors1[CIRCLE_NUM_POINTS/2*3];
	glm::vec2 lefteyeball_vertices1[CIRCLE_NUM_POINTS];
	glm::vec3 lefteyeball_colors1[CIRCLE_NUM_POINTS];
	glm::vec2 righteyeball_vertices1[CIRCLE_NUM_POINTS];
	glm::vec3 righteyeball_colors1[CIRCLE_NUM_POINTS];
	glm::vec2 mouth_vertices1[CIRCLE_NUM_POINTS/2*3];
	glm::vec3 mouth_colors1[CIRCLE_NUM_POINTS/2*3];
	glm::vec2 line1_vertices1[CIRCLE_NUM_POINTS];
	glm::vec3 line1_colors1[CIRCLE_NUM_POINTS];
	glm::vec2 line2_vertices1[6];
	glm::vec3 line2_colors1[6];

	// 定义第二个表情的顶点数组和颜色数组
	glm::vec2 face_vertices2[CIRCLE_NUM_POINTS+1];
	glm::vec3 face_colors2[CIRCLE_NUM_POINTS+1];
	glm::vec2 lefteye_vertices2[CIRCLE_NUM_POINTS/2*3];
	glm::vec3 lefteye_colors2[CIRCLE_NUM_POINTS/2*3];
	glm::vec2 righteye_vertices2[CIRCLE_NUM_POINTS/2*3];
	glm::vec3 righteye_colors2[CIRCLE_NUM_POINTS/2*3];
	glm::vec2 lefteyeball_vertices2[CIRCLE_NUM_POINTS];
	glm::vec3 lefteyeball_colors2[CIRCLE_NUM_POINTS];
	glm::vec2 righteyeball_vertices2[CIRCLE_NUM_POINTS];
	glm::vec3 righteyeball_colors2[CIRCLE_NUM_POINTS];
	glm::vec2 mouth_vertices2[CIRCLE_NUM_POINTS/2];
	glm::vec3 mouth_colors2[CIRCLE_NUM_POINTS/2];


	// 调用生成第一个表情顶点位置和颜色信息的函数
	generateFace1Points(face_vertices1, face_colors1, 0, glm::vec2{-0.5, 0.0});
	generateEyePoints(lefteye_vertices1, lefteye_colors1, 0, glm::vec2{-0.35, 0.25}, glm::vec2{-0.5, 0.0});
	generateEyePoints(righteye_vertices1, righteye_colors1, 0, glm::vec2{0.35, 0.25}, glm::vec2{-0.5, 0.0});
	generateEyeballPoints(lefteyeball_vertices1, lefteyeball_colors1, 0, glm::vec2{-0.35, 0.25}, glm::vec2{-0.5, 0.0});
	generateEyeballPoints(righteyeball_vertices1, righteyeball_colors1, 0, glm::vec2{0.35, 0.25}, glm::vec2{-0.5, 0.0});
	generateMouth1Points(mouth_vertices1, mouth_colors1, 0, glm::vec2{0, -0.25}, glm::vec2{-0.5, 0.0});
	generateLine1Points(line1_vertices1, line1_colors1, 0, glm::vec2{0, -0.25}, glm::vec2{-0.5, 0.0});
	generateLine2Points(line2_vertices1, line2_colors1, 0, glm::vec2{0, -0.25}, glm::vec2{-0.5, 0.0});

	// 调用生成第二个表情顶点位置和颜色信息的函数
	generateFace2Points(face_vertices2, face_colors2, 0, glm::vec2{0.5, 0.0});
	generateEyePoints(lefteye_vertices2, lefteye_colors2, 0, glm::vec2{-0.35, 0.25}, glm::vec2{0.5, 0.0});
	generateEyePoints(righteye_vertices2, righteye_colors2, 0, glm::vec2{0.35, 0.25}, glm::vec2{0.5, 0.0});
	generateEyeballPoints(lefteyeball_vertices2, lefteyeball_colors2, 0, glm::vec2{-0.35, 0.25}, glm::vec2{0.5, 0.0});
	generateEyeballPoints(righteyeball_vertices2, righteyeball_colors2, 0, glm::vec2{0.35, 0.25}, glm::vec2{0.5, 0.0});
	generateMouth2Points(mouth_vertices2, mouth_colors2, 0, glm::vec2{0, -0.25}, glm::vec2{0.5, 0.0});

	// 读取着色器并使用
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	// 创建顶点缓存对象，vbo[2]是因为我们将要使用两个缓存对象
 	GLuint vbo[2];

	// 初始化第一个表情顶点坐标和颜色信息
	transferringData(vao[0], vbo, face_vertices1, face_colors1, CIRCLE_NUM_POINTS);
	transferringData(vao[1], vbo, lefteye_vertices1, lefteye_colors1, CIRCLE_NUM_POINTS/2*3);
	transferringData(vao[2], vbo, righteye_vertices1, righteye_colors1, CIRCLE_NUM_POINTS/2*3);
	transferringData(vao[3], vbo, lefteyeball_vertices1, lefteyeball_colors1, CIRCLE_NUM_POINTS);
	transferringData(vao[4], vbo, righteyeball_vertices1, righteyeball_colors1, CIRCLE_NUM_POINTS);
	transferringData(vao[5], vbo, mouth_vertices1, mouth_colors1, CIRCLE_NUM_POINTS);
	transferringData(vao[6], vbo, line1_vertices1, line1_colors1, CIRCLE_NUM_POINTS);
	transferringData(vao[7], vbo, line2_vertices1, line2_colors1, 6);

	// 初始化第二个表情顶点坐标和颜色信息
	transferringData(vao[8], vbo, face_vertices2, face_colors2, CIRCLE_NUM_POINTS+1);
	transferringData(vao[9], vbo, lefteye_vertices2, lefteye_colors2, CIRCLE_NUM_POINTS/2*3);
	transferringData(vao[10], vbo, righteye_vertices2, righteye_colors2, CIRCLE_NUM_POINTS/2*3);
	transferringData(vao[11], vbo, lefteyeball_vertices2, lefteyeball_colors2, CIRCLE_NUM_POINTS);
	transferringData(vao[12], vbo, righteyeball_vertices2, righteyeball_colors2, CIRCLE_NUM_POINTS);
	transferringData(vao[13], vbo, mouth_vertices2, mouth_colors2, CIRCLE_NUM_POINTS/2);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

// 用于生成自己发挥想象绘制的图
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	// 绘制第一个表情
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS/2*3);
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS/2*3);
	glBindVertexArray(vao[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[4]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[5]); 
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[6]);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[7]);
	glLineWidth(7.0f);
	glDrawArrays(GL_LINES, 0, 6);

	// 绘制第二个表情
	glBindVertexArray(vao[8]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS+1);
	glBindVertexArray(vao[9]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS/2*3);
	glBindVertexArray(vao[10]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS/2*3);
	glBindVertexArray(vao[11]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[12]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);
	glBindVertexArray(vao[13]);
	glLineWidth(7.0f);
	glDrawArrays(GL_LINE_STRIP, 0, CIRCLE_NUM_POINTS/2);

	glFlush();
}

int main(int argc, char **argv)
{
	// 初始化GLFW库
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(512, 512, "2022150176_简校滨_实验一", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();

	std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	
	while (!glfwWindowShouldClose(window))
	{	
		display();
		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}
