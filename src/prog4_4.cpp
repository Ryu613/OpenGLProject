/*
    ̫��������������ת��תģ��(���þ���ջ)
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

// ������display()������ʹ�õı����ռ䣬�������ǾͲ�������Ⱦ�����з���
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
	// �Ѷ�����ص�vbo
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	// ÿ����z-buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	// ������Ҫÿ����color buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	// ��ȡMV���󣬺�ͶӰ�����uniform����
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// ������ͼ��ģ�;�������ɫ����
	// ��ͼ����ͨ������λ������ƽ�Ƶ��������ʵ��
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	// ����ͼ����ѹ�����ջ
	mvStack.push(vMat);
	// ��ͶӰ����д��glsl����
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// ��������׶̫��
	// ��ջ������ͼ������һ�ݣ���ѹ��ջ��top:ȡ��ջ�����ǲ�ɾ��
	mvStack.push(mvStack.top());
	// ջ���¸��Ƴ�������ͼ������̫����ƽ��ת���������
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	// ����һ�ݣ���ѹ��ջ��
	mvStack.push(mvStack.top());
	// ջ��(v*̫��ƽ��)����̫����ת�任�������(����glfw����ʱ�䣬��x����ת)
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	// ��ջ����(v*̫��ƽ��*̫����ת)����д�뵽glsl��mv������
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// ����̫��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 18);
	// ����ջ��(̫��mv����)
	mvStack.pop();

	// ��������������
	// �ѵ�ǰջ��(v*̫��ƽ��)����һ��ѹ��ջ��
	mvStack.push(mvStack.top());
	// ջ��(v*̫��ƽ��)����������ת�任�������(����glfw����ʱ�䣬��ƽ��)��ģ�⹫ת
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
	// ���Ʋ�ѹ��ջ��(v*����ƽ��)��Ŀǰջ�Ե�����Ϊ: ��ͼ����->��ͼ����*̫��ƽ��->��ͼ����*����ƽ��->��ͼ����*����ƽ��
	mvStack.push(mvStack.top());
	// ջ��(v*����ƽ��)����������ת�任�������(����glfw����ʱ�䣬��y����ת)��ģ����ת
	// Ŀǰջ�Ե�����Ϊ: ��ͼ����->��ͼ����* ̫��ƽ��->��ͼ����* ����ƽ��->����mv
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// ��������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// ����ջ����ջ�Ե�����Ϊ: ��ͼ����->��ͼ����* ̫��ƽ��->��ͼ����* ����ƽ��
	mvStack.pop();

	// ������������������
	// ����ջ����ѹ��ջ��
	mvStack.push(mvStack.top());
	// ģ�⹫ת
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	// ģ���Դ�
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	// �����������С
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	// Ŀǰջ��Ϊ����mv����
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	// ��������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// Ŀǰջ�Ե�����Ϊ: ��ͼ����->��ͼ����* ̫��ƽ��-> ��ͼ����* ����ƽ�� -> ����mv�� ȫ���������
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();  // the final pop is for the view matrix
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