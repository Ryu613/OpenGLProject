/*
    ���ļ���ȡglsl
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

#define numVAOs 1

GLuint renderingProgram;
// vao:�����������
GLuint vao[numVAOs];

using namespace std;

string readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

GLuint createShaderProgram() {
    // ���ļ���ȡshader
    string vertShaderStr = readShaderSource("shader/vertShader_p2_5.glsl");
    string fragShaderStr = readShaderSource("shader/fragShader_p2_5.glsl");

    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // ����ɫ��GLSL�������뵽����ɫ�������У������ǣ���ŵ���ɫ��������ɫ��Դ���е��ַ�������������Դ����ַ�����ָ�룬��δ�õ��Ĳ���NULL
    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    // ������ɫ��
    glCompileShader(vShader);
    glCompileShader(fShader);
    // ����������󣬴�������ID
    GLuint vfProgram = glCreateProgram();
    // ����ɫ�����뵽���������
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    // ����GLSL��������ȷ��������
    glLinkProgram(vfProgram);

    return vfProgram;
}

// ��ʼ��Ӧ�ó�����صĴ���
void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    // ����VAO�����ڴ��뻺�����������������ݼ����͵�һ����ʽ
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

// ������صĴ���
void display(GLFWwindow* window, double currentTime) {
    // ��֮ǰ����õĴ���2����ɫ���ĳ���������뵽OpenGL�Ĺ���(GPU��)
    glUseProgram(renderingProgram);
    // ���õ�Ĵ�СΪ30������
    glPointSize(30.0f);
    // �������ߴ���,���������� 
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program4", NULL, NULL);
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