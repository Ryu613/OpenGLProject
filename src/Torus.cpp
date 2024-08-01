#include "include/Torus.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

Torus::Torus() {
    prec = 48;
    inner = 0.5f;
    outer = 0.2f;
    init();
}

Torus::Torus(float innerRadius, float outerRadius, int precIn) {
    prec = precIn;
    inner = innerRadius;
    outer = outerRadius;
    init();
}

float Torus::toRadians(float degrees) {
    return (degrees * 2.0f * 3.14159f) / 360.f;
}

void Torus::init() {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    for (int i = 0; i < numVertices; i++) {
        vertices.push_back(glm::vec3());
        texCoords.push_back(glm::vec2());
        normals.push_back(glm::vec3());
        sTangents.push_back(glm::vec3());
        tTangents.push_back(glm::vec3());
    }
    //for (int i = 0; i < numVertices; i++) {
    //    vertices.push_back(glm::vec3());
    //}
    //for (int i = 0; i < numVertices; i++) {
    //    texCoords.push_back(glm::vec2());
    //}
    //for (int i = 0; i < numVertices; i++) {
    //    normals.push_back(glm::vec3());
    //}
    //for (int i = 0; i < numVertices; i++) {
    //    sTangents.push_back(glm::vec3());
    //}
    //for (int i = 0; i < numVertices; i++) {
    //    tTangents.push_back(glm::vec3());
    //}
    for (int i = 0; i < numIndices; i++) {
        indices.push_back(0);
    }

    // �����һ����
    for (int i = 0; i < prec + 1; i++) {
        float amt = toRadians(i * 360.f / prec);
        // ��ԭ����ת�㣬�γɻ���Ȼ�����������ƶ�
        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.f, 0.f, 1.f));
        glm::vec3 initPos(rMat * glm::vec4(0.f, outer, 0.f, 1.f));
        vertices[i] = glm::vec3(initPos + glm::vec3(inner, 0.f, 0.f));

        // Ϊ����ÿ�����������������
        texCoords[i] = glm::vec2(0.f, ((float)i / (float)prec));

        // �����������ͷ���������һ������������z����ת��y��
        rMat = glm::rotate(glm::mat4(1.f), amt + (3.14159f / 2.0f), glm::vec3(0.f, 0.f, 1.f));
        tTangents[i] = glm::vec3(rMat * glm::vec4(0.f, -1.f, 0.f, 1.f));
        sTangents[i] = glm::vec3(glm::vec3(0.f, 0.f, -1.f)); // �ڶ�����������-z��
        normals[i] = glm::cross(tTangents[i], sTangents[i]); // ���ǵĲ�����Ƿ�����
    }

    // ��y����ת������Ǹ������γ������Ļ�
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int vert = 0; vert < prec + 1; vert++) {
            // ��y����ת����Ǹ����Ķ�������
            float amt = (float)(toRadians(ring * 360.f / prec));
            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.f, 1.f, 0.f));
            vertices[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(vertices[vert], 1.f));
            // �����»��������������
            texCoords[ring * (prec + 1) + vert] = glm::vec2((float)ring * 2.0f / (float)prec, texCoords[vert].t);
            // ��y����ת�������͸�������
            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            sTangents[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(sTangents[vert], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            tTangents[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(tTangents[vert], 1.0f));
            // ��y����ת������
            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            normals[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(normals[vert], 1.0f));
        }
    }

    // Ϊ����������������μ�������
    for (int ring = 0; ring < prec; ring++) {
        for (int vert = 0; vert < prec; vert++) {
            indices[((ring * prec + vert) * 2) * 3 + 0] = ring * (prec + 1) + vert;
            indices[((ring * prec + vert) * 2) * 3 + 1] = (ring + 1) * (prec + 1) + vert;
            indices[((ring * prec + vert) * 2) * 3 + 2] = ring * (prec + 1) + vert + 1;
            indices[((ring * prec + vert) * 2 + 1) * 3 + 0] = ring * (prec + 1) + vert + 1;
            indices[((ring * prec + vert) * 2 + 1) * 3 + 1] = (ring + 1) * (prec + 1) + vert;
            indices[((ring * prec + vert) * 2 + 1) * 3 + 2] = (ring + 1) * (prec + 1) + vert + 1;
        }
    }
}
int Torus::getNumVertices() { return numVertices; }
int Torus::getNumIndices() { return numIndices; }
std::vector<int> Torus::getIndices() { return indices; }
std::vector<glm::vec3> Torus::getVertices() { return vertices; }
std::vector<glm::vec2> Torus::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Torus::getNormals() { return normals; }
std::vector<glm::vec3> Torus::getStangents() { return sTangents; }
std::vector<glm::vec3> Torus::getTtangents() { return tTangents; }