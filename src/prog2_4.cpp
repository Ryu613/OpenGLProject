/*
    从文件读取glsl
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

#define numVAOs 1

GLuint renderingProgram;
// vao:顶点数组对象
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
    // 从文件读取shader
    string vertShaderStr = readShaderSource("shader/vertShader_p2_5.glsl");
    string fragShaderStr = readShaderSource("shader/fragShader_p2_5.glsl");

    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 把着色器GLSL代码载入到空着色器对象中，参数是：存放的着色器对象，着色器源码中的字符串数量，包含源码的字符串的指针，暂未用到的参数NULL
    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    // 编译着色器
    glCompileShader(vShader);
    glCompileShader(fShader);
    // 创建程序对象，存下整数ID
    GLuint vfProgram = glCreateProgram();
    // 把着色器加入到程序对象中
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    // 请求GLSL编译器，确保兼容性
    glLinkProgram(vfProgram);

    return vfProgram;
}

// 初始化应用程序相关的代码
void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    // 创建VAO：用于存入缓冲区，缓冲区是数据集发送的一种形式
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

// 绘制相关的代码
void display(GLFWwindow* window, double currentTime) {
    // 把之前编译好的带着2个着色器的程序对象载入到OpenGL的管线(GPU里)
    glUseProgram(renderingProgram);
    // 设置点的大小为30个像素
    glPointSize(30.0f);
    // 启动管线处理,绘制三角形 
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program4", NULL, NULL);
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