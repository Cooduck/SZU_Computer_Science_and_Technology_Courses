#include "Camera.h"

Camera::Camera() {};
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->near, this->far);
	}
	else {
		return this->perspective(fov, aspect, this->near, this->far);
	}
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	// use glm.
	glm::vec3 eye_3 = eye;
	glm::vec3 at_3 = at;
	glm::vec3 up_3 = up;

	glm::mat4 view = glm::lookAt(eye_3, at_3, up_3);

	return view;
}


glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = -2.0 / (zFar - zNear);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);

	c = glm::transpose(c);
	return c;
}

glm::mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	GLfloat top = tan(fovy * M_PI / 180 / 2) * zNear;
	GLfloat right = top * aspect;

	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;

	c = glm::transpose(c);
	return c;
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

void Camera::changePerspectiveMode(int mode){
	this->mode = mode;
}

int Camera::getPerspectiveMode(){
	return this->mode;
}

void Camera::initCameraMode5(){
	this->cameraRotation = glm::vec3{0.0, 0.0, 0.0};
	this->cameraPos = this->stevePos + glm::vec3{0.0, 2.0, 0.0};
	this->cameraDir = glm::vec3{0.0, 0.0, 1.0};
}

void Camera::updateCamera()
{
	if(this->mode == 1){
		// 全局视角
		float eyex = radius * cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
		float eyey = radius * sin(upAngle * M_PI / 180.0);
		float eyez = radius * cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);
		eye = glm::vec4(eyex, eyey, eyez, 1.0);
		at = glm::vec4(0.0, 0.0, 0.0, 1.0);

		up = glm::vec4(0.0, 1.0, 0.0, 0.0);
		if (upAngle > 90){
			up.y = -1;
		}
		else if (upAngle < -90){
			up.y = -1;
		}
	}
	else if(this->mode == 2){
		// 第三人称视角1
		eye = glm::vec4(stevePos.x - steveDir.x * 4, stevePos.y + 0.4 + steveDir.y * 4, stevePos.z - steveDir.z * 4, 1.0);
		at = glm::vec4(stevePos.x + steveDir.x, stevePos.y + 0.4 - steveDir.y * 4, stevePos.z + steveDir.z, 1.0);
	}
	else if(this->mode == 3){
		// 第一人称视角
		eye = glm::vec4(stevePos.x, stevePos.y + 0.4, stevePos.z, 1.0);
		at = glm::vec4(stevePos.x + steveDir.x, stevePos.y + 0.4 - steveDir.y, stevePos.z + steveDir.z, 1.0);
	}
	else if(this->mode == 4){
		// 第三人称视角2
		eye = glm::vec4(stevePos.x + steveDir.x * 4, stevePos.y - steveDir.y * 4, stevePos.z + steveDir.z * 4, 1.0);
		at = glm::vec4(stevePos, 1.0);
	}
	else if(this->mode == 5){
		// 自由视角
		eye = glm::vec4(cameraPos, 1.0);
		at = glm::vec4(cameraPos + cameraDir * glm::vec3{2.0, 2.0, 2.0}, 1.0);
	}
}


void Camera::keyboard(int key, int action, int mode)
{
	// 键盘事件处理
	// 如果当前是全局视角模式
	if(this->mode == 1) {
		// 当按下 'U' 键且没有按住 Shift 键时，增加旋转角度
		if (key == GLFW_KEY_U && mode == 0x0000) {
			rotateAngle += 10.0;
		}
		// 当按下 'U' 键且按住 Shift 键时，减少旋转角度
		else if (key == GLFW_KEY_U && mode == GLFW_MOD_SHIFT) {
			rotateAngle -= 10.0;
		}
		// 当按下 'I' 键且没有按住 Shift 键时，增加视角的上下角度（限制最大值为 180）
		else if (key == GLFW_KEY_I && mode == 0x0000) {
			upAngle += 10.0;
			if (upAngle > 180)
				upAngle = 180;
		}
		// 当按下 'I' 键且按住 Shift 键时，减少视角的上下角度（限制最小值为 -180）
		else if (key == GLFW_KEY_I && mode == GLFW_MOD_SHIFT) {
			upAngle -= 10.0;
			if (upAngle < -180)
				upAngle = -180;
		}
		// 当按下 'O' 键且没有按住 Shift 键时，增加相机的半径（最大为 40）
		else if (key == GLFW_KEY_O && mode == 0x0000) {
			radius = glm::min(radius + 1.0f, 40.0f);
		}
		// 当按下 'O' 键且按住 Shift 键时，减少相机的半径（最小为 0）
		else if (key == GLFW_KEY_O && mode == GLFW_MOD_SHIFT) {
			radius = glm::max(radius - 1.0f, 0.0f);
		}
		// 当按下空格键时，重置所有相机参数为初始值
		else if (key == GLFW_KEY_SPACE && mode == 0x0000) {
			radius = 2.0;          // 初始半径
			rotateAngle = 0.0;     // 初始旋转角度
			upAngle = 0.0;         // 初始上下角度
			fov = 45.0;           // 初始视场角（FOV）
			aspect = 1.0;          // 初始宽高比
			scale = 1.5;           // 初始缩放比例
		}
	}
	// 如果当前是自由视角模式
	else if(this->mode == 5){
		glm::vec3 right = glm::normalize(glm::cross(cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
		// 如果按下 'W' 键
		if (key == GLFW_KEY_W && mode == 0x0000) {
			cameraPos += cameraDir * glm::vec3(0.5, 0.5, 0.5);  // 将相机位置沿着相机的方向移动
		}
		// 如果按下 'S' 键
		else if (key == GLFW_KEY_S && mode == 0x0000) {
			cameraPos -= cameraDir * glm::vec3(0.5, 0.5, 0.5);  // 将相机位置沿着相机的反方向移动
		}
		// 如果按下 'A' 键
		else if (key == GLFW_KEY_A && mode == 0x0000) {
			cameraPos -= right * glm::vec3(0.5, 0.5, 0.5);  // 向右移动，右向量方向
		}

		// 如果按下 'D' 键
		else if (key == GLFW_KEY_D && mode == 0x0000) {
			cameraPos += right * glm::vec3(0.5, 0.5, 0.5);  // 向左移动，右向量反方向
		}
	}

}

void Camera::mouse(float deltaX, float deltaY)
{
	// 如果当前是全局视角模式
    if(this->mode == 1) {
        // 如果鼠标在X轴上移动（deltaX > 0），增加旋转角度，反之减少旋转角度
        if(deltaX > 0){
            rotateAngle += abs(deltaX) * 0.1f;  // 鼠标横向移动时，旋转角度增大
        }
        else if(deltaX < 0){
            rotateAngle -= abs(deltaX) * 0.1f;  // 鼠标横向移动时，旋转角度减小
        }

        // 如果鼠标在Y轴上移动（deltaY > 0），减少上下角度（仰角），反之增加上下角度（俯角）
        if(deltaY > 0){
            upAngle -= abs(deltaY) * 0.1f;  // 鼠标纵向向上移动时，减小上下角度
            if (upAngle < -180)  // 限制最小角度为 -180°
                upAngle = -180;
        }
        else if(deltaY < 0){
            upAngle += abs(deltaY) * 0.1f;  // 鼠标纵向向下移动时，增大上下角度
            if (upAngle > 180)  // 限制最大角度为 180°
                upAngle = 180;
        }
    }
	// 如果当前是自由视角模式
	else if(this->mode == 5){ 
		// 根据鼠标水平移动，调整相机的旋转角度（绕Z轴旋转）
		if(deltaX > 0){
			cameraRotation.y -= abs(deltaX) * 0.1f;  // 鼠标向右滑动，顺时针旋转相机
		}
		else if(deltaX < 0){
			cameraRotation.y += abs(deltaX) * 0.1f;  // 鼠标向左滑动，逆时针旋转相机
		}

		// 根据鼠标竖直移动，调整相机的俯仰角度（绕Y轴旋转）
		if(deltaY > 0){
			cameraRotation.z -= abs(deltaY) * 0.1f;  // 鼠标向上滑动，向下俯视
			if (cameraRotation.z < -90)  // 限制俯仰角度，避免超过范围
				cameraRotation.z = -89;
		}
		else if(deltaY < 0){
			cameraRotation.z += abs(deltaY) * 0.1f;  // 鼠标向下滑动，向上仰视
			if (cameraRotation.z > 90)  // 限制俯仰角度，避免超过范围
				cameraRotation.z = 89;
		}

		// 通过旋转角度计算新的相机方向向量（cameraDir）
		cameraDir.x = cos(cameraRotation.z * M_PI / 180.0) * sin(cameraRotation.y * M_PI / 180.0);  // 计算X方向
		cameraDir.y = sin(cameraRotation.z * M_PI / 180.0);  // 计算Y方向
		cameraDir.z = cos(cameraRotation.z * M_PI / 180.0) * cos(cameraRotation.y * M_PI / 180.0);  // 计算Z方向
	}

}

void Camera::mouseScroll(double xoffset, double yoffset)
{
    // 如果当前是全局视角模式
    if(this->mode == 1){
        // 如果鼠标滚轮向上滚动（yoffset > 0），缩小相机的视距（radius）
        if (yoffset > 0) {
            // 缩小视距，确保最小为 0.0f
            radius = glm::max(radius - abs((float)yoffset) * 0.3f, 0.0f);
        }
        // 如果鼠标滚轮向下滚动（yoffset < 0），增大相机的视距（radius）
        else if(yoffset < 0) {
            // 增大视距，确保最大为 40.0f
            radius = glm::min(radius + abs((float)yoffset) * 0.3f, 40.0f);
        }
    }
	// 如果当前是自由视角模式
	else if(this->mode == 5){
		// 鼠标滚轮向下滚动（yoffset < 0），减少相机的Y轴高度
		if (yoffset < 0) {
			// 将相机的Y位置下降，最大下降到0.0f，避免相机位置低于地面
			cameraPos.y = glm::max(cameraPos.y - abs((float)yoffset) * 0.3f, 0.0f);
		}
		// 鼠标滚轮向上滚动（yoffset > 0），增加相机的Y轴高度
		else if(yoffset > 0) {
			// 将相机的Y位置上升，最大上升到40.0f，避免相机位置过高
			cameraPos.y = glm::min(cameraPos.y + abs((float)yoffset) * 0.3f, 40.0f);
		}
	}

}