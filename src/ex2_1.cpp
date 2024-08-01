/*
    把点周期性的放大缩小
    记住：每次将背景清除为黑色，否则变为最大时就看上去没动了，因为背景是最大时候的点的图像
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define numVAOs 1

GLuint renderingProgram;
// vao:顶点数组对象
GLuint vao[numVAOs];

float pointSize = 1.f; //点的大小
float pointOffset = 1.f; //点的每次尺寸变动

GLuint createShaderProgram() {
    // 430指OpenGL版本是4.3，gl_Position代表GLSL的内置变量，代表坐标
    // vec4四元数
    const char* vshaderSource = "#version 430 \n"
        "void main(void) \n"
        "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";
    // out 输出变量
    const char* fshaderSource = "#version 430 \n"
        "out vec4 color; \n"
        "void main(void) \n"
        //"{ color = vec4(0.0, 0.0, 1.0, 1.0); }";
        // gl_FragCoord: 访问输入片段的坐标，基于像素位置设置颜色
        "{ if(gl_FragCoord.x < 295) color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "else color = vec4(0.0, 0.0, 1.0, 1.0);}";
    // 创建顶点和片段着色器
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 把着色器GLSL代码载入到空着色器对象中，参数是：存放的着色器对象，着色器源码中的字符串数量，包含源码的字符串的指针，暂未用到的参数NULL
    glShaderSource(vShader, 1, &vshaderSource, NULL);
    glShaderSource(fShader, 1, &fshaderSource, NULL);
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

using namespace std;

// 初始化应用程序相关的代码
void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    // 创建VAO：用于存入缓冲区，缓冲区是数据集发送的一种形式
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

// 绘制相关的代码
void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT); // 每次清除深度缓冲
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); // 每次将背景清除为黑色
    // 把之前编译好的带着2个着色器的程序对象载入到OpenGL的管线(GPU里)
    glUseProgram(renderingProgram);
    if (pointSize == 30.0f || pointSize == 0.0f) {
        pointOffset = -pointOffset;
    }
    pointSize += pointOffset;
    glPointSize(pointSize);
    // 启动管线处理
    glDrawArrays(GL_POINTS, 0, 1);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program2", NULL, NULL);
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