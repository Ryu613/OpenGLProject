/*
    ����׶�������壺��4.1���ƣ�����׶���һ�鶥�㲢��һ���µ�vbo
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
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// ������display()������ʹ�õı����ռ䣬�������ǾͲ�������Ⱦ�����з���
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

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
	// ����vao
	glGenVertexArrays(1, vao);
	// ��vao����һ��
	glBindVertexArray(vao[0]);
	// ����vbo
	glGenBuffers(numVBOs, vbo);

	// ��vbo0
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// �Ѷ�������д��vbo��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	// ��vbo1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// �Ѷ�������д��vbo��
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("shader/vertShader_p4_1a.glsl", "shader/fragShader_p4_1a.glsl");
	// ���λ��
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 8.0f;
	// �������λ��(Y������һ����չʾ͸��Ч��)
	cubeLocX = 0.0f;
	cubeLocY = -2.0f;
	cubeLocZ = 0.0f;
	// ����׶λ��
	pyrLocX = 2.0f;
	pyrLocY = 2.0f;
	pyrLocZ = 0.0f;
	// �Ѷ�����ص�vbo
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	// ÿ����z-buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	// ������Ҫÿ����color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	// ��ȡMV���󣬺�ͶӰ�����uniform����
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// ����͸�Ӿ���
	// ��ȡglfw���ڵĿ��
	glfwGetFramebufferSize(window, &width, &height);
	// �����߱�
	aspect = (float)width / (float)height;
	// fov = 60�����㻡��Ϊ1.0472
	// pMat:͸�Ӿ���
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// ������ͼ��ģ�;�������ɫ����
	// ��ͼ����ͨ������λ������ƽ�Ƶ��������ʵ��
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	mvMat = mMat * vMat;
	// ��͸�Ӿ���v����, ʱ�����Ӹ�ֵ����Ӧ��uniform����
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	// ��VBO������������ɫ������Ӧ�Ķ�������
	// ��ǵ�0��������Ϊ��Ծ
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// ����0�����Թ���������������һ����������Ӧglsl��location=0��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// ���õ�0����������
	glEnableVertexAttribArray(0);

	// ����OpenGL���ã�����ģ��
	// ������Ȳ��Բ����ò��Է���
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// һ����6������(2��������)��6����36����
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// ��������׶
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	mvMat = mMat * vMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 18);
}

int main() {
	//��ʼ��glfw�⣬��ʧ�����˳�
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	// ʵ����glfwWindow��ָ�������������OpenGL 4.3����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// ����glfw���ڣ�����Ϊ�����ߣ����⣬����ȫ����ʾ����Դ����
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter4 - program3", NULL, NULL);
	// ��glfw�Ĵ��ں͵�ǰOpenGL�������Ĺ�������
	glfwMakeContextCurrent(window);
	// ��ʼ��glew��
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}
	// ˫����Ľ������ʱ��
	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		// ��glfw��ʼ���󾭹���ʱ�䣬��֤�ٲ�ͬ�����������ͬ�ٶȲ���
		display(window, glfwGetTime());
		// ����˫����
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}