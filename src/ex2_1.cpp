/*
    �ѵ������ԵķŴ���С
    ��ס��ÿ�ν��������Ϊ��ɫ�������Ϊ���ʱ�Ϳ���ȥû���ˣ���Ϊ���������ʱ��ĵ��ͼ��
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define numVAOs 1

GLuint renderingProgram;
// vao:�����������
GLuint vao[numVAOs];

float pointSize = 1.f; //��Ĵ�С
float pointOffset = 1.f; //���ÿ�γߴ�䶯

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
    glClear(GL_DEPTH_BUFFER_BIT); // ÿ�������Ȼ���
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); // ÿ�ν��������Ϊ��ɫ
    // ��֮ǰ����õĴ���2����ɫ���ĳ���������뵽OpenGL�Ĺ���(GPU��)
    glUseProgram(renderingProgram);
    if (pointSize == 30.0f || pointSize == 0.0f) {
        pointOffset = -pointOffset;
    }
    pointSize += pointOffset;
    glPointSize(pointSize);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program2", NULL, NULL);
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