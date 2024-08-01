/*
    �򵥶���,ÿ��display�ƶ������εĶ��������xֵ
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

float x = 0.0f; // ��������x���λ��
float inc = 0.01f; //�ƶ������ε�ƫ����

float frameTime = 0.0f; // ֡��Ⱦʱ��

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
    string vertShaderStr = readShaderSource("shader/vertShader_p2_6.glsl");
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
    glClear(GL_DEPTH_BUFFER_BIT); // ÿ�������Ȼ���
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); // ÿ�ν��������Ϊ��ɫ

    // ��֮ǰ����õĴ���2����ɫ���ĳ���������뵽OpenGL�Ĺ���(GPU��)
    glUseProgram(renderingProgram);
    // �����ٶȱ�׼Ϊһ��60��(������Ⱦ֡��)
    float animationFPS = 1 / 60.0f;
    // �����ٶȵı���
    float animationSpeedRate = 1.0f;
    float frameDuration = currentTime - frameTime;
    if (x > 1.0f) inc = -0.01f;
    if (x < -1.0f) inc = 0.01f;
    // �����ζ���ʱ�����ȶ���FPS���죬����Ⱦͼ�����һ��һ��
    if (frameDuration < animationFPS) {
        glDrawArrays(GL_TRIANGLES, 0, 3);
        return;
    }
    else {
        frameTime = currentTime;
        animationSpeedRate *= frameDuration / animationFPS;
    }

    x += inc * animationSpeedRate; // λ���� * �����ٶȱ���
    //if (x > 1.0f) inc = -0.01f;
    //if (x < -1.0f) inc = 0.01f;
    GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset"); // ��ȡָ��offset������ָ��
    glProgramUniform1f(renderingProgram, offsetLoc, x); // ��xֵ���Ƹ�offset
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - ex2.4", NULL, NULL);
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