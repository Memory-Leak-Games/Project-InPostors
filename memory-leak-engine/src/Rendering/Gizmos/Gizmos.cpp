#include "Rendering/Gizmos/Gizmos.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/ShaderAsset.h"

#include "Rendering/ShaderProgram.h"

using namespace mlg;

GLuint Gizmos::lineVAO;
GLuint Gizmos::lineVBO;
GLuint Gizmos::boxVAO;
GLuint Gizmos::boxVBO;
GLuint Gizmos::boxEBO;
GLuint Gizmos::sphereVAO;
GLuint Gizmos::sphereVBO;
GLuint Gizmos::sphereEBO;
GLuint Gizmos::sphereIndicesCount;
GLuint Gizmos::pointVBO;
GLuint Gizmos::pointVAO;

ShaderProgram* Gizmos::shader;
glm::vec4 Gizmos::defaultColor = {0, 1, 0, 1};

GLfloat LineVertices[]{
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f};

GLfloat BoxVertices[]{
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
};

GLuint BoxIndices[]{
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7};

// Yes, I also find this silly
GLfloat PointVertex[]{
        0.0f, 0.0f, 0.0f};

void mlg::Gizmos::Initialize() {
    // Load gizmo shader
    shader = new ShaderProgram (
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.vert"),
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.frag"));

    // Generate line gizmo
    {
        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), LineVertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &lineVAO);
        glBindVertexArray(lineVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate box gizmo
    {
        glGenBuffers(1, &boxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), BoxVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &boxEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), BoxIndices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &boxVAO);
        glBindVertexArray(boxVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLuint), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate sphere gizmo
    {
        std::vector<GLfloat> SphereVertices;
        std::vector<GLuint> SphereIndices;
        GenerateSphere(SphereVertices, SphereIndices, 30);

        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), &SphereVertices[0], GL_STATIC_DRAW);
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), &SphereIndices[0], GL_STATIC_DRAW);
        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLuint), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate point gizmo
    {
        glGenBuffers(1, &pointVBO);
        glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), PointVertex, GL_STATIC_DRAW);
        glGenVertexArrays(1, &pointVAO);
        glBindVertexArray(pointVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Unbind
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void mlg::Gizmos::Stop() {
    if (shader == nullptr)
        return;


    //TODO: delete vaos and vbos
}

void mlg::Gizmos::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, bool alwaysFront) {
    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, start);
    world = glm::scale(world, end - start);// Use position diff to scale line (which is from [0, 0, 0] to [1, 1, 1])

    shader->Activate();
    shader->SetMat4F("world", world);
    shader->SetVec4F("color", color);
    shader->SetBool("alwaysFront", alwaysFront);

    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(Transform& transform, glm::vec4 color, bool alwaysFront) {
    const glm::mat4 world = transform.GetLocalMatrix();

    shader->Activate();
    shader->SetMat4F("world", world);
    shader->SetVec4F("color", color);
    shader->SetBool("alwaysFront", alwaysFront);

    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(glm::vec3 position, glm::vec3 size, glm::quat rotation, glm::vec4 color, bool alwaysFront) {
    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, position);
    world = world * glm::toMat4(rotation);
    world = glm::scale(world, size);// Default size is 1x1x1

    shader->Activate();
    shader->SetMat4F("world", world);
    shader->SetVec4F("color", color);
    shader->SetBool("alwaysFront", alwaysFront);

    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawSphere(glm::vec3 position, float radius, glm::vec4 color, bool alwaysFront) {
    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, position);
    world = glm::scale(world, glm::vec3(radius * 2));// Default radius is 1

    shader->Activate();
    shader->SetMat4F("world", world);
    shader->SetVec4F("color", color);
    shader->SetBool("alwaysFront", alwaysFront);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glDrawElements(GL_LINES, sphereIndicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawPoint(glm::vec3 position, glm::vec4 color, bool alwaysFront) {
    DrawSizedPoint(position, 2, color, alwaysFront);
}

void Gizmos::DrawSizedPoint(glm::vec3 position, float size, glm::vec4 color, bool alwaysFront) {
    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, position);

    shader->Activate();
    shader->SetMat4F("world", world);
    shader->SetVec4F("color", color);
    shader->SetBool("alwaysFront", alwaysFront);
    shader->SetFloat("pointSize", size);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(pointVAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

// FIXME: It's not a sphere :/
void Gizmos::GenerateSphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int LOD) {
    int i, j;
    int indicator = 0;
    for (i = 0; i <= LOD; i++) {
        double lat0 = glm::pi<double>() * (-0.5 + (double) (i - 1) / LOD);
        double z0 = glm::sin(lat0);
        double zr0 = glm::cos(lat0);

        double lat1 = glm::pi<double>() * (-0.5 + (double) i / LOD);
        double z1 = glm::sin(lat1);
        double zr1 = glm::cos(lat1);

        for (j = 0; j <= LOD; j++) {
            double lng = 2 * glm::pi<double>() * (double) (j - 1) / LOD;
            double x = glm::cos(lng);
            double y = glm::sin(lng);

            vertices.push_back(x * zr0 * 0.5);
            vertices.push_back(y * zr0 * 0.5);
            vertices.push_back(z0 * 0.5);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x * zr1 * 0.5);
            vertices.push_back(y * zr1 * 0.5);
            vertices.push_back(z1 * 0.5);
            indices.push_back(indicator);
            indicator++;
        }
    }

    sphereIndicesCount = indices.size();
}
