/*
    ��һ����
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

// ��ʼ��Ӧ�ó�����صĴ���
void init(GLFWwindow* window) {

}

// ������صĴ���
void display(GLFWwindow* window, double currentTime) {
    // �趨���ɫΪ��ɫ��r,g,b,a��
    glClearColor(1.0, 0.0, 0.0, 1.0);
    // ʹ�����ɫ������Ⱦ���ͼ�����ɫ���������
    glClear(GL_COLOR_BUFFER_BIT);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program1", NULL, NULL);
    // ��glfw�Ĵ��ں͵�ǰOpenGL�������Ĺ�������
    glfwMakeContextCurrent(window);
    // ��ʼ��glew��
    if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        // ��glfw��ʼ���󾭹���ʱ�䣬��֤�ٲ�ͬ�����������ͬ�ٶȲ���
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}