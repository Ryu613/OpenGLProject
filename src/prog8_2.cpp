/*
    柔和阴影PCF实现
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/Torus.hpp"
#include "include/Utils.h"
#include "include/ImportedModel.h"

#define numVAOs 1
#define numVBOs 5

GLuint renderingProgram1, renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

Torus myTorus(0.5f, 0.2f, 48);
ImportedModel pyramid("model/pyr.obj");
int numPyramidVertices, numTorusVertices, numTorusIndices;

GLuint mvLoc, projLoc, nLoc, sLoc;

GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;

int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 currentLightPos, lightPosV; //在模型和视觉空间中的光照位置
float lightPos[3]; // 光照位置
glm::vec3 origin(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

// 环面，四棱锥，相机，光源位置
glm::vec3 torusLoc(1.6f, 0.0f, -0.3f);
glm::vec3 pyrLoc(-1.0f, 0.1f, 0.3f);
glm::vec3 cameraLoc(0.0f, 0.2f, 6.0f);
glm::vec3 lightLoc(-3.8f, 2.2f, 1.1f);

// 白光特性
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// 四棱锥黄金材质特性
float* goldMatAmb = Utils::goldAmbient();
float* goldMatDif = Utils::goldDiffuse();
float* goldMatSpe = Utils::goldSpecular();
float goldMatShi = Utils::goldShininess();

// 环面的青铜材质
float* bronzeMatAmb = Utils::bronzeAmbient();
float* bronzeMatDif = Utils::bronzeDiffuse();
float* bronzeMatSpe = Utils::bronzeSpecular();
float  bronzeMatShi = Utils::bronzeShininess();

// 用于在display()中将光照传入着色器的变量
float curAmb[4], curDif[4], curSpe[4], matAmb[4], matDif[4], matSpe[4];
float curShi, matShi;

// 阴影相关变量
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void setupVertices(void) {
    // 四棱锥
    numPyramidVertices = pyramid.getNumVertices();
    std::vector<glm::vec3> vert = pyramid.getVertices();
    std::vector<glm::vec3> norm = pyramid.getNormals();

    std::vector<float> pyramidPvalues;
    std::vector<float> pyramidNvalues;

    for (int i = 0; i < numPyramidVertices; i++) {
        pyramidPvalues.push_back((vert[i]).x);
        pyramidPvalues.push_back((vert[i]).y);
        pyramidPvalues.push_back((vert[i]).z);
        pyramidNvalues.push_back((norm[i]).x);
        pyramidNvalues.push_back((norm[i]).y);
        pyramidNvalues.push_back((norm[i]).z);
    }
    // 圆环
    numTorusVertices = myTorus.getNumVertices();
    numTorusIndices = myTorus.getNumIndices();
    std::vector<int> ind = myTorus.getIndices();
    vert = myTorus.getVertices();
    norm = myTorus.getNormals();

    std::vector<float> torusPvalues;
    std::vector<float> torusNvalues;

    for (int i = 0; i < numTorusVertices; i++) {
        torusPvalues.push_back(vert[i].x);
        torusPvalues.push_back(vert[i].y);
        torusPvalues.push_back(vert[i].z);
        torusNvalues.push_back(norm[i].x);
        torusNvalues.push_back(norm[i].y);
        torusNvalues.push_back(norm[i].z);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, torusPvalues.size() * 4, &torusPvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, pyramidPvalues.size() * 4, &pyramidPvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, torusNvalues.size() * 4, &torusNvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, pyramidNvalues.size() * 4, &pyramidNvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}

void setupShadowBuffers(GLFWwindow* window) {


    glfwGetFramebufferSize(window, &width, &height);
    screenSizeX = width;
    screenSizeY = height;

    // 创建自定义帧缓冲区
    glGenFramebuffers(1, &shadowBuffer);

    // 创建引用纹理并让它存储深度信息
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // 解决重复阴影问题
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void init(GLFWwindow* window) {
    renderingProgram1 = Utils::createShaderProgram("shader/vertShader1_p8_2.glsl", "shader/fragShader1_p8_2.glsl");
    renderingProgram2 = Utils::createShaderProgram("shader/vertShader2_p8_2.glsl", "shader/fragShader2_p8_2.glsl");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    setupVertices();
    setupShadowBuffers(window);
    b = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    );
}

void installLights(int renderingProgram, glm::mat4 vMatrix) {
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    matAmb[0] = curAmb[0]; matAmb[1] = curAmb[1]; matAmb[2] = curAmb[2]; matAmb[3] = curAmb[3];
    matDif[0] = curDif[0]; matDif[1] = curDif[1]; matDif[2] = curDif[2]; matDif[3] = curDif[3];
    matSpe[0] = curSpe[0]; matSpe[1] = curSpe[1]; matSpe[2] = curSpe[2]; matSpe[3] = curSpe[3];
    matShi = curShi;

    // get the locations of the light and material fields in the shader
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");
    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    //  set the uniform light and material values in the shader
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, matShi);
}

void passOne() {
    glUseProgram(renderingProgram1);

    // draw the torus

    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
    sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

    // draw the pyramid

    mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
    mMat = glm::rotate(mMat, toRadians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}

void passTwo() {
    glUseProgram(renderingProgram2);

    mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
    sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

    // draw the torus

    curAmb[0] = bronzeMatAmb[0]; curAmb[1] = bronzeMatAmb[1]; curAmb[2] = bronzeMatAmb[2];  // bronze
    curDif[0] = bronzeMatDif[0]; curDif[1] = bronzeMatDif[1]; curDif[2] = bronzeMatDif[2];
    curSpe[0] = bronzeMatSpe[0]; curSpe[1] = bronzeMatSpe[1]; curSpe[2] = bronzeMatSpe[2];
    curShi = bronzeMatShi;

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));

    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    currentLightPos = glm::vec3(lightLoc);
    installLights(renderingProgram2, vMat);

    mvMat = vMat * mMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));
    shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

    // draw the pyramid

    curAmb[0] = goldMatAmb[0]; curAmb[1] = goldMatAmb[1]; curAmb[2] = goldMatAmb[2];  // gold
    curDif[0] = goldMatDif[0]; curDif[1] = goldMatDif[1]; curDif[2] = goldMatDif[2];
    curSpe[0] = goldMatSpe[0]; curSpe[1] = goldMatSpe[1]; curSpe[2] = goldMatSpe[2];
    curShi = goldMatShi;

    mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
    mMat = glm::rotate(mMat, toRadians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    currentLightPos = glm::vec3(lightLoc);
    installLights(renderingProgram2, vMat);

    mvMat = vMat * mMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));
    shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    currentLightPos = glm::vec3(lightLoc);

    lightVmatrix = glm::lookAt(currentLightPos, origin, up);
    lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);	// for reducing
    glPolygonOffset(2.0f, 4.0f);		//  shadow artifacts

    passOne();

    glDisable(GL_POLYGON_OFFSET_FILL);	// artifact reduction, continued

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    glDrawBuffer(GL_FRONT);

    passTwo();
}

int main() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter8 - program 2", NULL, NULL);
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