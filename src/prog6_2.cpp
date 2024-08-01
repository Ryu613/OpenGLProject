/*
    程序生成环体
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/Torus.hpp"
#include "include/Utils.h"

#define numVAOs 1
#define numVBOs 4

float cameraX, cameraY, cameraZ;
float torLocX, torLocY, torLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint brickTexture;
Torus myTorus(0.5f, 0.2f, 48);

GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void setupVertices(void) {
    std::vector<int> ind = myTorus.getIndices();
    std::vector<glm::vec3> vert = myTorus.getVertices();
    std::vector<glm::vec2> tex = myTorus.getTexCoords();
    std::vector<glm::vec3> norm = myTorus.getNormals();

    std::vector<float> pvalues; // 顶点位置
    std::vector<float> tvalues; // 纹理坐标
    std::vector<float> nvalues; // 法向量


    for (int i = 0; i < myTorus.getNumVertices(); i++) {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        tvalues.push_back(tex[i].s);
        tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    // 把顶点放入缓冲区0
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

    // 把纹理放入缓冲区1
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

    // 把法向量放入缓冲区2
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

    // 索引放入缓冲区3
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("shader/vertShader_p6_1.glsl", "shader/fragShader_p6_1.glsl");
    // 构建透视矩阵
    // 获取glfw窗口的宽高
    glfwGetFramebufferSize(window, &width, &height);
    // 计算宽高比
    aspect = (float)width / (float)height;
    // fov = 60，换算弧度为1.0472
    // pMat:透视矩阵
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    // 相机位置
    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 2.0f;
    torLocX = 0.0f;
    torLocY = 0.0f;
    torLocZ = -0.5f;
    // 加载纹理
    brickTexture = Utils::loadTexture("texture/brick1.jpg");
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // 把顶点加载到vbo
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
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
    mMat = glm::rotate(mMat, toRadians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mvMat = vMat * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter6 - program2", NULL, NULL);
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