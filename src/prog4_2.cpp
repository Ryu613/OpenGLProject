/*
	3d立方体（颜色渐变）24个,使用实例化方式
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
using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// 分配在display()函数中使用的变量空间，这样它们就不必在渲染过程中分配
GLuint vLoc, projLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat;
float timeFactor;

void setupVertices() {
	float vertexPositions[108] = {
	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	// 生成vao
	glGenVertexArrays(1, vao);
	// 绑定vao到第一个
	glBindVertexArray(vao[0]);
	// 生成vbo
	glGenBuffers(numVBOs, vbo);
	// 绑定vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// 把顶点数据写到vbo中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("shader/vertShader_p4_2.glsl", "shader/fragShader_p4_2.glsl");
	// 相机位置
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 8.0f;
	// 立方体的位置(Y轴下移一点以展示透视效果)
	cubeLocX = 0.0f;
	cubeLocY = -2.0f;
	cubeLocZ = 0.0f;
	// 把顶点加载到vbo
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	// 每次清z-buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	// 动画需要每次清color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	//时间因子
	timeFactor = (float)currentTime;
	// 获取MV矩阵，和投影矩阵的uniform变量
	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	tfLoc = glGetUniformLocation(renderingProgram, "tf");

	// 构建透视矩阵
	// 获取glfw窗口的宽高
	glfwGetFramebufferSize(window, &width, &height);
	// 计算宽高比
	aspect = (float)width / (float)height;
	// fov = 60，换算弧度为1.0472
	// pMat:透视矩阵
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// 构建视图，模型矩阵在着色器里
	// 视图矩阵：通过将单位矩阵反向平移到相机坐标实现
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	// 将透视矩阵，v矩阵, 时间因子赋值给对应的uniform变量
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniform1f(tfLoc, timeFactor);
	// 将VBO关联给顶点着色器中相应的顶点属性
	// 标记第0个缓冲区为活跃
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// 将第0个属性关联到缓冲区（第一个参数，对应glsl中location=0）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// 启用第0个顶点属性
	glEnableVertexAttribArray(0);

	// 调整OpenGL设置，绘制模型
	// 启用深度测试并设置测试方法
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// 一个面6个顶点(2个三角形)，6个面36顶点
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 24);
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
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter4 - program2", NULL, NULL);
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