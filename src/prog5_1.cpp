/*
	砖纹理四棱锥
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/Utils.h"
#include <stack>
#include <SOIL2/SOIL2.h>

using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// 分配在display()函数中使用的变量空间，这样它们就不必在渲染过程中分配
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
GLuint brickTexture;

void setupVertices() {
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	// 纹理坐标顶点，每三个元素代表一个顶点的xyz
	float pyrTexCoords[36] =
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	// 生成vao
	glGenVertexArrays(1, vao);
	// 绑定vao到第一个
	glBindVertexArray(vao[0]);
	// 生成vbo
	glGenBuffers(numVBOs, vbo);

	// 绑定vbo0
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// 把顶点数据写到vbo中
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
	// 绑定vbo1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 把顶点数据写到vbo中
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("shader/vertShader_p5_1.glsl", "shader/fragShader_p5_1.glsl");
	// 构建透视矩阵
	// 获取glfw窗口的宽高
	glfwGetFramebufferSize(window, &width, &height);
	// 计算宽高比
	aspect = (float)width / (float)height;
	// fov = 60，换算弧度为1.0472
	// pMat:透视矩阵
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	// 相机位置
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 12.0f;
	// 加载纹理
	brickTexture = Utils::loadTexture("texture/brick1.jpg");
	// 把顶点加载到vbo
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	// 旋转一点方便看到立体效果
	mMat = glm::rotate(mMat, -0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.61f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.00f, glm::vec3(0.0f, 0.0f, 1.0f));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	// 激活并绑定第0个纹理单元
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 18);
}

int main() {
	//初始化glfw库，若失败则退出
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	// 实例化glfwWindow，指定计算机必须与OpenGL 4.3兼容
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 创建glfw窗口，参数为（宽，高，标题，允许全屏显示，资源共享）
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter5 - program1", NULL, NULL);
	// 把glfw的窗口和当前OpenGL的上下文关联起来
	glfwMakeContextCurrent(window);
	// 初始化glew库
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}
	// 双缓冲的交换间隔时间
	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		// 用glfw初始化后经过的时间，保证再不同计算机上以相同速度播放
		display(window, glfwGetTime());
		// 开启双缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}