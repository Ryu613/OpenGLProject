/*
    太阳，地球，月亮自转公转模拟(利用矩阵栈)
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
using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// 分配在display()函数中使用的变量空间，这样它们就不必在渲染过程中分配
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
stack<glm::mat4> mvStack;

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
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	// 绑定vbo1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 把顶点数据写到vbo中
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("shader/vertShader_p4_1a.glsl", "shader/fragShader_p4_1a.glsl");
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
	// 把顶点加载到vbo
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	// 每次清z-buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	// 动画需要每次清color buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	// 获取MV矩阵，和投影矩阵的uniform变量
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// 构建视图，模型矩阵在着色器里
	// 视图矩阵：通过将单位矩阵反向平移到相机坐标实现
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	// 把视图矩阵压入矩阵栈
	mvStack.push(vMat);
	// 把投影矩阵写进glsl变量
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// 绘制四棱锥太阳
	// 把栈顶的视图矩阵复制一份，再压入栈顶top:取出栈顶但是不删除
	mvStack.push(mvStack.top());
	// 栈顶新复制出来的视图矩阵与太阳的平移转换矩阵相乘
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	// 复制一份，再压入栈顶
	mvStack.push(mvStack.top());
	// 栈顶(v*太阳平移)再与太阳旋转变换矩阵相乘(根据glfw窗口时间，绕x轴旋转)
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	// 把栈顶的(v*太阳平移*太阳旋转)矩阵写入到glsl的mv矩阵中
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// 绘制太阳
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 18);
	// 弹出栈顶(太阳mv矩阵)
	mvStack.pop();

	// 绘制立方体行星
	// 把当前栈顶(v*太阳平移)复制一份压入栈顶
	mvStack.push(mvStack.top());
	// 栈顶(v*太阳平移)再与行星旋转变换矩阵相乘(根据glfw窗口时间，来平移)，模拟公转
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
	// 复制并压入栈顶(v*行星平移)，目前栈自底向上为: 视图矩阵->视图矩阵*太阳平移->视图矩阵*行星平移->视图矩阵*行星平移
	mvStack.push(mvStack.top());
	// 栈顶(v*行星平移)再与行星旋转变换矩阵相乘(根据glfw窗口时间，绕y轴旋转)，模拟自转
	// 目前栈自底向上为: 视图矩阵->视图矩阵* 太阳平移->视图矩阵* 行星平移->行星mv
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// 绘制行星
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// 弹出栈顶后，栈自底向上为: 视图矩阵->视图矩阵* 太阳平移->视图矩阵* 行星平移
	mvStack.pop();

	// 绘制立方体卫星月球
	// 复制栈顶并压入栈顶
	mvStack.push(mvStack.top());
	// 模拟公转
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	// 模拟自传
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	// 调整立方体大小
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	// 目前栈顶为月球mv矩阵
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// 绘制月球
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// 目前栈自底向上为: 视图矩阵->视图矩阵* 太阳平移-> 视图矩阵* 行星平移 -> 行星mv， 全部弹出清除
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();  // the final pop is for the view matrix
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
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter4 - program3", NULL, NULL);
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