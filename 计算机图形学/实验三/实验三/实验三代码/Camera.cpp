#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
    // 计算视线向量（视点到目标点）
    glm::vec3 vpn = glm::vec3(eye - at); // 视点到目标的向量
    glm::vec3 n = glm::normalize(vpn); // 归一化，得到视线方向（前向）

    // 计算右向量（相机的右侧方向）
    glm::vec3 u = glm::normalize(glm::cross(glm::vec3(up), n)); // 用上方向和前向的叉积得到右向量

    // 计算上向量（重新计算上方向，确保正交性）
    glm::vec3 v = glm::normalize(glm::cross(n, u)); // 通过前向和右向的叉积得到新的上向量

    // 构造视图矩阵
    glm::mat4 viewMatrix = {
        glm::vec4(u, -glm::dot(u, glm::vec3(eye))),  // 第一列：右向量，右移量为相机位置的点积
        glm::vec4(v, -glm::dot(v, glm::vec3(eye))),  // 第二列：上向量，上移量为相机位置的点积
        glm::vec4(n, -glm::dot(n, glm::vec3(eye))),  // 第三列：前向量，前移量为相机位置的点积
        glm::vec4(0, 0, 0, 1)                          // 第四列：齐次坐标，保持为 (0, 0, 0, 1)
    };

    // 转置矩阵以符合列主序存储
    viewMatrix = glm::transpose(viewMatrix);
    return viewMatrix; // 返回计算出的观察矩阵
}

glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	else {
		return this->perspective(fov, aspect, this->zNear, this->zFar);
	}
}

glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
    const GLfloat bottom, const GLfloat top,
    const GLfloat zNear, const GLfloat zFar)
{
    // 构造正交投影矩阵
    glm::mat4 orthoMatrix = {
        glm::vec4(2.0f / (right - left), 0, 0, -(right + left) / (right - left)), // x 轴
        glm::vec4(0, 2.0f / (top - bottom), 0, -(top + bottom) / (top - bottom)), // y 轴
        glm::vec4(0, 0, -2.0f / (zFar - zNear), -(zFar + zNear) / (zFar - zNear)), // z 轴
        glm::vec4(0, 0, 0, 1) // 齐次坐标
    };

    // 转置矩阵以符合列主序存储
    orthoMatrix = glm::transpose(orthoMatrix);
    return orthoMatrix; // 返回计算出的正交投影矩阵
}

glm::mat4 Camera::perspective(const GLfloat fov, const GLfloat aspect,
    const GLfloat zNear, const GLfloat zFar)
{
    // 计算上边界和右边界
    float top = zNear * tan(glm::radians(fov / 2.0f)); // 使用焦距和视场角计算上边界
    float right = top * aspect; // 根据宽高比计算右边界

    // 构造透视投影矩阵
    glm::mat4 perspectiveMatrix = {
        glm::vec4(zNear / right, 0, 0, 0),
        glm::vec4(0, zNear / top, 0, 0),
        glm::vec4(0, 0, -(zFar + zNear) / (zFar - zNear), -(2.0f * zFar * zNear) / (zFar - zNear)),
        glm::vec4(0, 0, -1, 0)
    };

    // 转置矩阵以符合列主序存储
    perspectiveMatrix = glm::transpose(perspectiveMatrix);
    return perspectiveMatrix; // 返回计算出的透视投影矩阵
}

glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 任意视锥体矩阵
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;

	c = glm::transpose(c);
	return c;
}


void Camera::updateCamera()
{
    // 初始化up向量，表示相机的上方向
	up = glm::vec4(0.0, 1.0, 0.0, 0.0);
	
	// 根据upAngle调整up向量的y分量
	if (upAngle > 90) {
		up.y = -1;  // 如果upAngle大于90度，将up向量的y分量设置为-1，表示相机的上方向相反
	}
	else if (upAngle < -90) {
		up.y = -1;  // 如果upAngle小于-90度，也设置up向量的y分量为-1
	}

    // 计算相机的位置（eye）在球坐标系中的坐标
	float eyex = radius * cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
	float eyey = radius * sin(upAngle * M_PI / 180.0);
	float eyez = radius * cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);

    // 将计算得到的相机位置赋值给eye向量
	eye = glm::vec4(eyex, eyey, eyez, 1.0);

    // 设置目标点的位置（at向量），即相机要观察的中心点
	at = glm::vec4(0.0, 0.7, 0.0, 1.0);  // 目标点固定在(0, 0.7, 0)，可以根据需要调整
}



void Camera::keyboard(int key, int action, int mode)
{
	if (key == GLFW_KEY_U && action == GLFW_PRESS && mode == 0x0000)
	{
		rotateAngle += 5.0;
	}
	else if (key == GLFW_KEY_U && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		rotateAngle -= 5.0;
	}
	else if (key == GLFW_KEY_I && action == GLFW_PRESS && mode == 0x0000)
	{
		upAngle += 5.0;
		if (upAngle > 180)
			upAngle = 180;
	}
	else if (key == GLFW_KEY_I && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		upAngle -= 5.0;
		if (upAngle < -180)
			upAngle = -180;
	}
	else if(key == GLFW_KEY_O && action == GLFW_PRESS && mode == 0x0000)
	{
		scale += 0.1;
	}
	else if(key == GLFW_KEY_O && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		scale -= 0.1;
	}
	else if (key == GLFW_KEY_J && action == GLFW_PRESS && mode == 0x0000)
	{
		radius += 0.1;
	}
	else if (key == GLFW_KEY_J && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		radius -= 0.1;
	}
	else if(key == GLFW_KEY_K && action == GLFW_PRESS && mode == 0x0000)
	{
		fov += 5.0;
	}
	else if(key == GLFW_KEY_K && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		fov -= 5.0;
	}
	else if(key == GLFW_KEY_L && action == GLFW_PRESS && mode == 0x0000)
	{
		aspect += 0.1;
	}
	else if(key == GLFW_KEY_L && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		aspect -= 0.1;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000)
	{
		radius = 4.0;
		rotateAngle = 0.0;
		upAngle = 0.0;
		fov = 45.0;
		aspect = 1.0;
		scale = 1.5;
	}
	
}
