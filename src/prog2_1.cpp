/*
    画一个点
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

// 初始化应用程序相关的代码
void init(GLFWwindow* window) {

}

// 绘制相关的代码
void display(GLFWwindow* window, double currentTime) {
    // 设定清除色为红色（r,g,b,a）
    glClearColor(1.0, 0.0, 0.0, 1.0);
    // 使用清除色来把渲染后的图像的颜色缓冲区清除
    glClear(GL_COLOR_BUFFER_BIT);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program1", NULL, NULL);
    // 把glfw的窗口和当前OpenGL的上下文关联起来
    glfwMakeContextCurrent(window);
    // 初始化glew库
    if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        // 用glfw初始化后经过的时间，保证再不同计算机上以相同速度播放
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}