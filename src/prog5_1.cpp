/*
	ש��������׶
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

// ������display()������ʹ�õı����ռ䣬�������ǾͲ�������Ⱦ�����з���
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
	// �������궥�㣬ÿ����Ԫ�ش���һ�������xyz
	float pyrTexCoords[36] =
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
	// ��vbo1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// �Ѷ�������д��vbo��
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("shader/vertShader_p5_1.glsl", "shader/fragShader_p5_1.glsl");
	// ����͸�Ӿ���
	// ��ȡglfw���ڵĿ��
	glfwGetFramebufferSize(window, &width, &height);
	// �����߱�
	aspect = (float)width / (float)height;
	// fov = 60�����㻡��Ϊ1.0472
	// pMat:͸�Ӿ���
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	// ���λ��
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 12.0f;
	// ��������
	brickTexture = Utils::loadTexture("texture/brick1.jpg");
	// �Ѷ�����ص�vbo
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
	// ��תһ�㷽�㿴������Ч��
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
	// ����󶨵�0������Ԫ
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);
	// ������Ȳ���
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
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter5 - program1", NULL, NULL);
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