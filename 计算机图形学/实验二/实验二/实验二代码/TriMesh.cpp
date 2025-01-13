#include "TriMesh.h"

TriMesh::TriMesh()
{
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

void TriMesh::cleanData() {
	vertex_positions.clear();
	vertex_colors.clear();	
	
	faces.clear();

	points.clear();
	colors.clear();
}

void TriMesh::storeFacesPoints() {
    // @TODO: Task-2修改此函数在points和colors容器中存储每个三角面片的各个点和颜色信息
    // 根据每个三角面片的顶点下标存储要传入GPU的数据

    // 遍历每个面片
	for(auto face : faces) {
        // 获取当前面片的三个顶点位置
        glm::vec3 v0 = vertex_positions[face.x]; // 第一个顶点
        glm::vec3 v1 = vertex_positions[face.y]; // 第二个顶点
        glm::vec3 v2 = vertex_positions[face.z]; // 第三个顶点

        // 根据顶点的坐标值进行颜色渐变，映射范围到0~1之间
        glm::vec3 color0 = glm::vec3(
            (v0.x + 1.0f) / 2.0f,  // x 轴映射到红色通道
            (v0.y + 1.0f) / 2.0f,  // y 轴映射到绿色通道
            (v0.z + 1.0f) / 2.0f); // z 轴映射到蓝色通道

        glm::vec3 color1 = glm::vec3(
            (v1.x + 1.0f) / 2.0f,  
            (v1.y + 1.0f) / 2.0f,  
            (v1.z + 1.0f) / 2.0f);

        glm::vec3 color2 = glm::vec3(
            (v2.x + 1.0f) / 2.0f,  
            (v2.y + 1.0f) / 2.0f,  
            (v2.z + 1.0f) / 2.0f);

        // 将顶点和对应的颜色分别存入points和colors中
        points.emplace_back(v0); // 存储第一个顶点位置
        points.emplace_back(v1); // 存储第二个顶点位置
        points.emplace_back(v2); // 存储第三个顶点位置
        colors.emplace_back(color0);  // 为第一个顶点分配渐变颜色
        colors.emplace_back(color1);  // 为第二个顶点分配渐变颜色
        colors.emplace_back(color2);  // 为第三个顶点分配渐变颜色
	}
}


void TriMesh::readOff(const std::string& filename)
{
    // 检查文件名是否为空
    if (filename.empty())
    {
        return; // 如果为空则返回
    }
    
    std::ifstream fin; // 文件输入流
    fin.open(filename); // 打开文件
    if (!fin) // 检查文件是否成功打开
    {
        printf("File on error\n");
        return; // 打开失败则返回
    }
    else
    {
        printf("File open success\n");
        cleanData(); // 清空之前的数据
        int nVertices, nFaces, nEdges;

        // 读取OFF文件的标识字符串
        std::string str;
        fin >> str; // 读取OFF字符串
        fin >> nVertices >> nFaces >> nEdges;   // 读取顶点数、面片数和边数
        
        // 根据顶点数，循环读取每个顶点坐标
        for (int i = 0; i < nVertices; i++)
        {
            glm::vec3 tmp_node; // 临时变量存储顶点坐标
            fin >> tmp_node.x >> tmp_node.y >> tmp_node.z; // 读取顶点坐标
            vertex_positions.push_back(tmp_node); // 存储顶点坐标
            vertex_colors.push_back(tmp_node); // 初始化顶点颜色（可以在后续处理中修改）
        }
        
        // 根据面片数，循环读取每个面片信息，并用vec3i结构体保存
        for (int i = 0; i < nFaces; i++)
        {
            int num, a, b, c; // num为顶点数量，a、b、c为顶点索引
            fin >> num >> a >> b >> c; // 读取面片的顶点索引
            faces.push_back(vec3i(a, b, c)); // 存储面片索引
        }
    }
    fin.close(); // 关闭文件
    storeFacesPoints(); // 存储面片和点的信息以供后续使用
}
