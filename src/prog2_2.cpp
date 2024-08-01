/*
    ����glsl
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define numVAOs 1

GLuint renderingProgram;
// vao:�����������
GLuint vao[numVAOs];

GLuint createShaderProgram() {
    // 430ָOpenGL�汾��4.3��gl_Position����GLSL�����ñ�������������
    // vec4��Ԫ��
    const char* vshaderSource = "#version 430 \n"
        "void main(void) \n"
        "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";
    // out �������
    const char* fshaderSource = "#version 430 \n"
        "out vec4 color; \n"
        "void main(void) \n"
        //"{ color = vec4(0.0, 0.0, 1.0, 1.0); }";
        // gl_FragCoord: ��������Ƭ�ε����꣬��������λ��������ɫ
        "{ if(gl_FragCoord.x < 295) color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "else color = vec4(0.0, 0.0, 1.0, 1.0);}";
    // ���������Ƭ����ɫ��
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // ����ɫ��GLSL�������뵽����ɫ�������У������ǣ���ŵ���ɫ��������ɫ��Դ���е��ַ�������������Դ����ַ�����ָ�룬��δ�õ��Ĳ���NULL
    glShaderSource(vShader, 1, &vshaderSource, NULL);
    glShaderSource(fShader, 1, &fshaderSource, NULL);
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

using namespace std;

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
    // �������ߴ���
    glDrawArrays(GL_POINTS, 0, 1);
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