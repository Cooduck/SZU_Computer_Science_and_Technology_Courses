#include "TriMesh.h"
#include <string.h>
#include <algorithm>
#include <thread>
#include <future>

// 一些基础颜色
const glm::vec3 basic_colors[8] = {
	glm::vec3(1.0, 1.0, 1.0), // White
	glm::vec3(1.0, 1.0, 0.0), // Yellow
	glm::vec3(0.0, 1.0, 0.0), // Green
	glm::vec3(0.0, 1.0, 1.0), // Cyan
	glm::vec3(1.0, 0.0, 1.0), // Magenta
	glm::vec3(1.0, 0.0, 0.0), // Red
	glm::vec3(0.0, 0.0, 0.0), // Black
	glm::vec3(0.0, 0.0, 1.0)	 // Blue
};

// 立方体的各个点
const glm::vec3 cube_vertices[8] = {
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(-0.5, 0.5, -0.5),
	glm::vec3(0.5, 0.5, -0.5),
	glm::vec3(-0.5, -0.5, 0.5),
	glm::vec3(0.5, -0.5, 0.5),
	glm::vec3(-0.5, 0.5, 0.5),
	glm::vec3(0.5, 0.5, 0.5)};

// 三角形的点
const glm::vec3 triangle_vertices[3] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.0, 0.5, 0.0)};

// 正方形平面
const glm::vec3 square_vertices[4] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.5, 0.5, 0.0),
	glm::vec3(-0.5, 0.5, 0.0),
};

TriMesh::TriMesh()
{
	do_normalize_size = true;
	diagonal_length = 1.0;
}

TriMesh::~TriMesh()
{
}

std::vector<glm::vec3> TriMesh::getVertexPositions()
{
	return vertex_positions;
}

std::vector<glm::vec3> TriMesh::getVertexColors()
{
	return vertex_colors;
}

std::vector<glm::vec3> TriMesh::getVertexNormals()
{
	return vertex_normals;
}

std::vector<glm::vec2> TriMesh::getVertexTextures()
{
	return vertex_textures;
}

std::vector<vec3i> TriMesh::getFaces()
{
	return faces;
}

std::vector<glm::vec3> TriMesh::getPoints()
{
	return points;
}

std::vector<glm::vec3> TriMesh::getColors()
{
	return colors;
}

std::vector<glm::vec3> TriMesh::getNormals()
{
	return normals;
}

std::vector<glm::vec2> TriMesh::getTextures()
{
	return textures;
}

void TriMesh::computeTriangleNormals()
{
	face_normals.resize(faces.size());
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		glm::vec3 v01 = vertex_positions[face.y] - vertex_positions[face.x];
		glm::vec3 v02 = vertex_positions[face.z] - vertex_positions[face.x];
		face_normals[i] = normalize(cross(v01, v02));
	}
}

void TriMesh::computeVertexNormals()
{
	// 计算面片的法向量
	if (face_normals.size() == 0 && faces.size() > 0)
	{
		computeTriangleNormals();
	}
	// 初始化法向量为0
	vertex_normals.resize(vertex_positions.size(), glm::vec3(0, 0, 0));
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		vertex_normals[face.x] += face_normals[i];
		vertex_normals[face.y] += face_normals[i];
		vertex_normals[face.z] += face_normals[i];
	}
	for (size_t i = 0; i < vertex_normals.size(); i++)
	{
		vertex_normals[i] = normalize(vertex_normals[i]);
	}
	// 球心在原点的球法向量为坐标
	// for (int i = 0; i < vertex_positions.size(); i++)
	// 	vertex_normals.push_back(vertex_positions[i] - vec3(0.0, 0.0, 0.0));
}

void TriMesh::setNormalize(bool do_norm) { do_normalize_size = true; }
bool TriMesh::getNormalize() { return do_normalize_size; }
float TriMesh::getDiagonalLength() { return diagonal_length; }

glm::vec3 TriMesh::getTranslation()
{
	return translation;
}

glm::vec3 TriMesh::getRotation()
{
	return rotation;
}

glm::vec3 TriMesh::getScale()
{
	return scale;
}

glm::mat4 TriMesh::getModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 trans = getTranslation();
	model = glm::translate(model, getTranslation());
	model = glm::rotate(model, glm::radians(getRotation()[2]), glm::vec3(0.0, 0.0, 1.0));
	model = glm::rotate(model, glm::radians(getRotation()[1]), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(getRotation()[0]), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, getScale());
	return model;
}

void TriMesh::setTranslation(glm::vec3 translation)
{
	this->translation = translation;
}

void TriMesh::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void TriMesh::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

glm::vec4 TriMesh::getAmbient() { return ambient; };
glm::vec4 TriMesh::getDiffuse() { return diffuse; };
glm::vec4 TriMesh::getSpecular() { return specular; };
float TriMesh::getShininess() { return shininess; };

void TriMesh::setAmbient(glm::vec4 _ambient) { ambient = _ambient; };
void TriMesh::setDiffuse(glm::vec4 _diffuse) { diffuse = _diffuse; };
void TriMesh::setSpecular(glm::vec4 _specular) { specular = _specular; };
void TriMesh::setShininess(float _shininess) { shininess = _shininess; };

bool TriMesh::getIsDraw() { return isDraw; }
void TriMesh::setIsDraw(bool flag) { isDraw = flag; }

bool TriMesh::getDrawShade() { return drawShade; }
void TriMesh::setDrawShade(bool flag) { drawShade = flag; }

void TriMesh::cleanData()
{
	vertex_positions.clear();
	vertex_colors.clear();
	vertex_normals.clear();
	vertex_textures.clear();

	faces.clear();
	normal_index.clear();
	color_index.clear();
	texture_index.clear();

	face_normals.clear();


	points.clear();
	colors.clear();
	normals.clear();
	textures.clear();
}

void TriMesh::storeFacesPoints()
{
	// 将读取的顶点根据三角面片上的顶点下标逐个加入
	// 要传递给GPU的points等容器内

	// 看是否归一化物体大小，是的话，这里将物体顶点缩放到对角线长度为1的包围盒内
	if (do_normalize_size) {
		// 记录物体包围盒大小，可以用于大小的归一化
		// 先获得包围盒的对角顶点
		float max_x = -FLT_MAX;
		float max_y = -FLT_MAX;
		float max_z = -FLT_MAX;
		float min_x = FLT_MAX;
		float min_y = FLT_MAX;
		float min_z = FLT_MAX;
		for (int i = 0; i < vertex_positions.size(); i++) {
			auto &position = vertex_positions[i];
			if (position.x > max_x ) max_x = position.x;
			if (position.y > max_y ) max_y = position.y;
			if (position.z > max_z ) max_z = position.z;
			if (position.x < min_x ) min_x = position.x;
			if (position.y < min_y ) min_y = position.y;
			if (position.z < min_z ) min_z = position.z;
		}
		up_corner = glm::vec3(max_x, max_y, max_z);
		down_corner = glm::vec3(min_x, min_y, min_z);
		center = glm::vec3((min_x+max_x)/2.0, (min_y+max_y)/2.0, (min_z+max_z)/2.0);

		diagonal_length = length( up_corner - down_corner );

		for (int i = 0; i < vertex_positions.size(); i++) {
			vertex_positions[i] = (vertex_positions[i] - center) / diagonal_length;
		}
	}

	// 计算法向量
	if (vertex_normals.size() == 0)
		computeVertexNormals();

	// @TODO Task2 根据每个三角面片的顶点下标存储要传入GPU的数据
	// 预先分配内存，避免动态增长容器导致的性能损失
	points.reserve(faces.size() * 3);
	colors.reserve(color_index.size() * 3);
	normals.reserve(normal_index.size() * 3); 
	textures.reserve(texture_index.size() * 3);  

	// 定义处理顶点数据的函数
	auto processPoint = [&](){
		// 遍历所有的三角面片，将每个面片的3个顶点坐标添加到 points 容器中
		for (int i = 0; i < faces.size(); i++){
			points.push_back(vertex_positions[faces[i].x]);  // 添加第一个顶点的坐标
			points.push_back(vertex_positions[faces[i].y]);  // 添加第二个顶点的坐标
			points.push_back(vertex_positions[faces[i].z]);  // 添加第三个顶点的坐标
		}
	};

	// 定义处理颜色数据的函数
	auto processColor = [&](){
		// 如果颜色索引不为空，说明有颜色数据
		if(!color_index.empty()){
			// 遍历所有的三角面片，将每个面片的3个顶点颜色添加到 colors 容器中
			for (int i = 0; i < faces.size(); i++){
				colors.push_back(vertex_colors[color_index[i].x]);  // 添加第一个顶点的颜色
				colors.push_back(vertex_colors[color_index[i].y]);  // 添加第二个顶点的颜色
				colors.push_back(vertex_colors[color_index[i].z]);  // 添加第三个顶点的颜色
			}
		}
	};

	// 定义处理法向量数据的函数
	auto processNormal = [&](){
		// 如果法向量索引不为空，说明有法向量数据
		if(!normal_index.empty()){
			// 遍历所有的三角面片，将每个面片的3个顶点的法向量添加到 normals 容器中
			for (int i = 0; i < faces.size(); i++){
				normals.push_back(vertex_normals[normal_index[i].x]);  // 添加第一个顶点的法向量
				normals.push_back(vertex_normals[normal_index[i].y]);  // 添加第二个顶点的法向量
				normals.push_back(vertex_normals[normal_index[i].z]);  // 添加第三个顶点的法向量
			}
		}
	};

	// 定义处理纹理坐标数据的函数
	auto processTexture = [&](){
		// 如果纹理索引不为空，说明有纹理数据
		if(!texture_index.empty()){
			// 遍历所有的三角面片，将每个面片的3个顶点的纹理坐标添加到 textures 容器中
			for (int i = 0; i < faces.size(); i++){
				textures.push_back(vertex_textures[texture_index[i].x]);  // 添加第一个顶点的纹理坐标
				textures.push_back(vertex_textures[texture_index[i].y]);  // 添加第二个顶点的纹理坐标
				textures.push_back(vertex_textures[texture_index[i].z]);  // 添加第三个顶点的纹理坐标
			}
		}
	};

	// 使用异步任务并行化数据处理的操作
	auto future1 = std::async(std::launch::async, processPoint);  // 异步执行处理顶点坐标的函数
	auto future2 = std::async(std::launch::async, processColor);  // 异步执行处理颜色数据的函数
	auto future3 = std::async(std::launch::async, processNormal);  // 异步执行处理法向量数据的函数
	auto future4 = std::async(std::launch::async, processTexture);  // 异步执行处理纹理坐标的函数

	// 等待所有异步任务完成，确保所有数据都处理完毕后再继续执行
	future1.get();  // 等待处理顶点的任务完成
	future2.get();  // 等待处理颜色的任务完成
	future3.get();  // 等待处理法向量的任务完成
	future4.get();  // 等待处理纹理坐标的任务完成
}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube()
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 8; i++)
	{
		vertex_positions.push_back(cube_vertices[i]);
		vertex_colors.push_back(basic_colors[i]);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 3));
	faces.push_back(vec3i(0, 3, 2));
	faces.push_back(vec3i(1, 4, 5));
	faces.push_back(vec3i(1, 0, 4));
	faces.push_back(vec3i(4, 0, 2));
	faces.push_back(vec3i(4, 2, 6));
	faces.push_back(vec3i(5, 6, 4));
	faces.push_back(vec3i(5, 7, 6));
	faces.push_back(vec3i(2, 6, 7));
	faces.push_back(vec3i(2, 7, 3));
	faces.push_back(vec3i(1, 5, 7));
	faces.push_back(vec3i(1, 7, 3));

	// 顶点纹理坐标，这里是每个面都用一个正方形图片的情况
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
}

void TriMesh::generateTriangle(glm::vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 3; i++)
	{
		vertex_positions.push_back(triangle_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));

	// 顶点纹理坐标
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0.5, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
}

void TriMesh::generateSquare(glm::vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 4; i++)
	{
		vertex_positions.push_back(square_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));
	faces.push_back(vec3i(0, 2, 3));

	// 顶点纹理坐标
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
}

void TriMesh::generateCylinder(int num_division, float radius, float height)
{

	cleanData();

	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度

	// 按cos和sin生成x，y坐标，z为负，即得到下表面顶点坐标
	// 顶点， 纹理
	float z = -height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		// 添加顶点坐标
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)));
		// 这里颜色和法向量一样
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)));
	}

	// 按cos和sin生成x，y坐标，z为正，即得到上表面顶点坐标
	z = height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)));
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)));
	}

	// 面片生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		faces.push_back(vec3i(i, (i + 1) % num_samples, (i) + num_samples));
		// 面片2
		faces.push_back(vec3i((i) + num_samples, (i + 1) % num_samples, (i + num_samples + 1) % (num_samples) + num_samples));

		// 面片1对应的顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 1.0));
		// 对应的三角面片的纹理坐标的下标
		texture_index.push_back( vec3i( 6*i, 6*i+1, 6*i+2 ) );

		// 面片2对应的顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 1.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 1.0));
		// 对应的三角面片的纹理坐标的下标
		texture_index.push_back( vec3i( 6*i+3, 6*i+4, 6*i+5 ) );
	}

	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;

	storeFacesPoints();
}

void TriMesh::generateDisk(int num_division, float radius)
{
	cleanData();
	
	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度

	// 按cos和sin生成x，y坐标，z为负，即得到下表面顶点坐标
	float z = 0;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back(glm::vec3(0, 0, 1));
		// 这里采用法线来生成颜色，可以自定义自己的颜色生成方式
		vertex_colors.push_back(glm::vec3(0, 0, 1));
	}
	// 中心点
	vertex_positions.push_back(glm::vec3(0, 0, 0));
	vertex_normals.push_back(glm::vec3(0, 0, 1));
	vertex_colors.push_back(glm::vec3(0, 0, 1));

	// 生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		faces.push_back(vec3i(i, (i + 1) % num_samples, num_samples));

		// 将0-360度映射到UV坐标的0-1
		for (int j = 0; j < 2; j++)
		{
			float r_r_r = (i + j) * step;
			float x = cos(r_r_r) / 2.0 + 0.5;
			float y = sin(r_r_r) / 2.0 + 0.5;
			// 纹理坐标
			vertex_textures.push_back(glm::vec2(x, y));
		}
		// 纹理坐标 中心
		vertex_textures.push_back(glm::vec2(0.5, 0.5));

		// 对应的三角面片的每个顶点的纹理坐标的下标
		texture_index.push_back(vec3i(3 * i, 3 * i + 1, 3 * i + 2));
	}

	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;

	
	storeFacesPoints();
}

void TriMesh::generateCone(int num_division, float radius, float height)
{
	cleanData();

	int num_samples = num_division;

	float step = 2 * M_PI / num_samples;

	float z = 0;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);

		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)) );
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)) );
	}

	vertex_positions.push_back(glm::vec3(0, 0, height));
	vertex_normals.push_back(glm::vec3(0, 0, 1));
	vertex_colors.push_back(glm::vec3(0, 0, 1));

	for (int i = 0; i < (num_samples); i++)
	{
		// 三角面片
		faces.push_back( vec3i( num_samples, (i) % (num_samples), (i + 1) % (num_samples) ) );
		// 该面片每个顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(0.5, 1-0));
		vertex_textures.push_back(glm::vec2( 1.0 * (i) / num_samples , 1-1));
		vertex_textures.push_back(glm::vec2( 1.0 * (i + 1) / num_samples , 1-1));
		// 该三角面片的每个顶点的纹理坐标的下标
		texture_index.push_back(vec3i( 3*i, 3*i+1, 3*i+2 ));
	}


	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;

	storeFacesPoints();
}

void TriMesh::readOff(const std::string &filename)
{
	// fin打开文件读取文件信息
	if (filename.empty())
	{
		return;
	}
	std::ifstream fin;
	fin.open(filename);
	if (!fin)
	{
		printf("File on error\n");
		return;
	}
	else
	{
		printf("File open success\n");

		cleanData();

		int nVertices, nFaces, nEdges;

		// 读取OFF字符串
		std::string str;
		fin >> str;
		// 读取文件中顶点数、面片数、边数
		fin >> nVertices >> nFaces >> nEdges;
		// 根据顶点数，循环读取每个顶点坐标
		for (int i = 0; i < nVertices; i++)
		{
			glm::vec3 tmp_node;
			fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
			vertex_positions.push_back(tmp_node);
			vertex_colors.push_back(tmp_node);
		}
		// 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存
		for (int i = 0; i < nFaces; i++)
		{
			int num, a, b, c;
			// num记录此面片由几个顶点构成，a、b、c为构成该面片顶点序号
			fin >> num >> a >> b >> c;
			faces.push_back(vec3i(a, b, c));
		}
	}
	fin.close();

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
};


void TriMesh::readObj(const std::string& filename)
{
    // 打开文件并检查是否成功
	std::ifstream fin(filename);
	std::string line;

	if (!fin)
	{
		std::cout << "ERROR: cannot open the file: " << filename << std::endl;
		exit(0);	// 如果文件打不开，退出程序
	}

	cleanData(); // 清除之前的数据（如果有）

	// 逐行读取文件
	while (std::getline(fin, line))
	{
		// 使用字符串流解析每一行
		std::istringstream sin(line);
		std::string type; // 存储行的类型（例如: v, vn, vt, f）
		GLfloat _x, _y, _z; // 存储顶点或纹理或法向量的坐标
		char slash; // 用于解析面（f）行中的斜杠
		sin >> type; // 获取行的类型

		// 处理顶点坐标行（v）
		if(type == "v"){
			sin >> _x >> _y >> _z; // 读取x, y, z坐标
			vertex_positions.push_back({_x, _y, _z}); // 将顶点坐标加入到顶点列表
		}
		// 处理法向量行（vn）
		else if(type == "vn"){
			sin >> _x >> _y >> _z; // 读取法向量的x, y, z分量
			vertex_normals.push_back({_x, _y, _z}); // 将法向量加入到法向量列表
			vertex_colors.push_back({_x, _y, _z}); // 用法向量的分量作为颜色值
		}
		// 处理纹理坐标行（vt）
		else if(type == "vt"){
			sin >> _x >> _y; // 读取纹理坐标
			vertex_textures.push_back({_x, _y}); // 将纹理坐标加入纹理坐标列表
		}
		// 处理面（f），该行描述了一个或多个面，可能是三角形或四边形
		// 需要提取顶点、法向量和纹理坐标的索引
		else if(type == "f"){
			std::vector<int> v, t, n; // 存储顶点、纹理和法向量的索引
			char vtnAllBuff[300]; // 用于保存读取的整个面数据
			sscanf(line.c_str(), "f%[^\n]", vtnAllBuff); // 读取整个面数据（去掉行首的 "f"）
			int spaceCount = std::count(vtnAllBuff, vtnAllBuff + strlen(vtnAllBuff), ' '); // 计算面中顶点的个数
			std::string regex = "%d/%d/%d"; // 面的格式为 顶点索引/纹理索引/法向量索引

			char *ptr = vtnAllBuff;
			// 解析面中的每一个顶点信息
			for (int i = 0; i < spaceCount; i++)
			{
				ptr += strcspn(ptr, " ") + 1; // 找到下一个顶点数据的位置
				int a, b, c;
				// 使用 sscanf 解析每个顶点的信息
				int ret = sscanf(ptr, regex.c_str(), &a, &b, &c);
				if (ret >= 1)
					v.push_back(a); // 存储顶点索引
				if (ret >= 2)
					t.push_back(b); // 存储纹理坐标索引
				if (ret >= 3)
					n.push_back(c); // 存储法向量索引
			}

			int lenth = v.size(); // 面中顶点的数量
			if(lenth <= 3){
				// 如果是三角形
				faces.push_back(vec3i(v[0] - 1, v[1] - 1, v[2] - 1)); // 存储三角形面
				if(!t.empty()){
					texture_index.push_back(vec3i(t[0] - 1, t[1] - 1, t[2] - 1)); // 存储纹理索引
				}
				if(!n.empty()){
					normal_index.push_back(vec3i(n[0] - 1, n[1] - 1, n[2] - 1)); // 存储法向量索引
					color_index.push_back(vec3i(n[0] - 1, n[1] - 1, n[2] - 1)); // 存储颜色索引（使用法向量）
				}
			}
			else{
				// 如果是四边形，拆分为两个三角形
				faces.push_back(vec3i(v[0] - 1, v[1] - 1, v[2] - 1)); // 第一个三角形
				faces.push_back(vec3i(v[2] - 1, v[3] - 1, v[0] - 1)); // 第二个三角形
				if(!t.empty()){
					texture_index.push_back(vec3i(t[0] - 1, t[1] - 1, t[2] - 1)); // 存储第一个三角形的纹理索引
					texture_index.push_back(vec3i(t[2] - 1, t[3] - 1, t[0] - 1)); // 存储第二个三角形的纹理索引
				}
				if(!n.empty()){
					normal_index.push_back(vec3i(n[0] - 1, n[1] - 1, n[2] - 1)); // 存储第一个三角形的法向量索引
					normal_index.push_back(vec3i(n[2] - 1, n[3] - 1, n[0] - 1)); // 存储第二个三角形的法向量索引
					color_index.push_back(vec3i(n[0] - 1, n[1] - 1, n[2] - 1)); // 存储第一个三角形的颜色索引
					color_index.push_back(vec3i(n[2] - 1, n[3] - 1, n[0] - 1)); // 存储第二个三角形的颜色索引
				}
			}
		}
	}

	// 调用函数处理面数据，将顶点、纹理、法向量等数据存储到模型中
	storeFacesPoints();
}


// 计算光源的阴影投影矩阵，基于光源的位置和模型矩阵
glm::mat4 Light::getShadowProjectionMatrix() {
    // 获取光源的模型矩阵
    glm::mat4 modelMatrix = this->getModelMatrix();
    // 计算光源的世界坐标（位置）
    glm::vec4 light_position = modelMatrix * glm::vec4(this->translation, 1.0);

    // 计算阴影矩阵
    glm::mat4 shadowMatrix = {
        glm::vec4(-light_position.y, light_position.x, 0, 0), // 第一行：根据光源位置设置阴影矩阵
        glm::vec4(0, 0, 0, 0), // 第二行：无变化
        glm::vec4(0, light_position.z, -light_position.y, 0), // 第三行：根据光源z位置设置阴影矩阵
        glm::vec4(0, 1, 0, -light_position.y), // 第四行：调整阴影偏移
    };
    return shadowMatrix; // 返回计算出的阴影矩阵
}

// 设置衰减系数的参数
void Light::setAttenuation(float _constant, float _linear, float _quadratic)
{
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
}

float Light::getConstant() { return constant; };
float Light::getLinear() { return linear; };
float Light::getQuadratic() { return quadratic; };
