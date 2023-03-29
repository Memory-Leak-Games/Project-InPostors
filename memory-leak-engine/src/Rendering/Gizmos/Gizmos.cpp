#include "Rendering/Gizmos/Gizmos.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/ShaderAsset.h"

#include "Rendering/ShaderProgram.h"
#include "Core/Time.h"

using namespace mlg;

uint32_t Gizmos::lineVAO;
uint32_t Gizmos::lineVBO;
uint32_t Gizmos::boxVAO;
uint32_t Gizmos::boxVBO;
uint32_t Gizmos::boxEBO;
uint32_t Gizmos::sphereVAO;
uint32_t Gizmos::sphereVBO;
uint32_t Gizmos::sphereEBO;
int32_t Gizmos::sphereIndicesCount;
uint32_t Gizmos::pointVBO;
uint32_t Gizmos::pointVAO;

ShaderProgram* Gizmos::shader;
glm::vec4 Gizmos::defaultColor = {0, 1, 0, 1};

std::vector<Gizmos::GizmoObject> Gizmos::gizmoInstances;

GLfloat lineVertices[]{
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f};

GLfloat boxVertices[]{
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
};

GLuint boxIndices[]{
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
};

// Yes, I also find this silly
GLfloat pointVertex[]{
        0.0f, 0.0f, 0.0f};

void mlg::Gizmos::Initialize() {
    // Load gizmo shader
    shader = new ShaderProgram(
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.vert"),
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.frag"));

    // Generate line gizmo
    {
        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), lineVertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &lineVAO);
        glBindVertexArray(lineVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate box gizmo
    {
        glGenBuffers(1, &boxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), boxVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &boxEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), boxIndices, GL_STATIC_DRAW);
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
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), pointVertex, GL_STATIC_DRAW);
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
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteBuffers(1, &boxEBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &pointVAO);
}

void Gizmos::DrawGizmos() {
    glEnable(GL_DEPTH_TEST);

    shader->Activate();
    glEnable(GL_PROGRAM_POINT_SIZE);
    auto gizmoEnd = gizmoInstances.end();
    for (auto gizmoIt = gizmoInstances.begin(); gizmoIt < gizmoEnd; gizmoIt++) {
        GizmoObject gizmo = *gizmoIt;
        shader->SetMat4F("world", gizmo.world);
        shader->SetVec4F("color", gizmo.color);
        shader->SetBool("alwaysFront", gizmo.alwaysFront);
        shader->SetFloat("pointSize", gizmo.pointSize);

        glBindVertexArray(gizmo.VAO);
        if (gizmo.EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmo.EBO);
            glDrawElements(gizmo.primitive, gizmo.elements, GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(gizmo.primitive, 0, gizmo.elements);
        }
    }

    gizmoInstances.clear();

    glDisable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mlg::Gizmos::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, bool alwaysFront) {
    GizmoObject gizmo;

    gizmo.VBO = lineVBO;
    gizmo.VAO = lineVAO;
    gizmo.elements = 2;

    gizmo.world = glm::mat4(1.0f);
    gizmo.world = glm::translate(gizmo.world, start);
    gizmo.world = glm::scale(gizmo.world,
                             end - start); // Use position diff to scale line (which is from [0, 0, 0] to [1, 1, 1])

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawBox(glm::vec3 position, glm::vec3 size, glm::quat rotation, glm::vec4 color, bool alwaysFront) {
    GizmoObject gizmo;

    gizmo.VBO = boxVBO;
    gizmo.VAO = boxVAO;
    gizmo.EBO = boxEBO;
    gizmo.elements = 24;

    gizmo.world = glm::mat4(1.0f);
    gizmo.world = glm::translate(gizmo.world, position);
    gizmo.world = gizmo.world * glm::toMat4(rotation);
    gizmo.world = glm::scale(gizmo.world, size); // Default size is 1x1x1

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawBox(Transform& transform, glm::vec4 color, bool alwaysFront) {
    GizmoObject gizmo;

    gizmo.VBO = boxVBO;
    gizmo.VAO = boxVAO;
    gizmo.EBO = boxEBO;
    gizmo.elements = 24;

    gizmo.world = transform.GetLocalMatrix();

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawSphere(glm::vec3 position, float radius, glm::vec4 color, bool alwaysFront) {
    GizmoObject gizmo;

    gizmo.VBO = sphereVBO;
    gizmo.VAO = sphereVAO;
    gizmo.EBO = sphereEBO;
    gizmo.elements = sphereIndicesCount;

    gizmo.world = glm::mat4(1.0f);
    gizmo.world = glm::translate(gizmo.world, position);
    gizmo.world = glm::scale(gizmo.world, glm::vec3(radius * 2)); // Default radius is 1

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;

    gizmoInstances.push_back(gizmo);

}

void Gizmos::DrawPoint(glm::vec3 position, glm::vec4 color, bool alwaysFront) {
    DrawSizedPoint(position, 2, color, alwaysFront);
}

void Gizmos::DrawSizedPoint(glm::vec3 position, float pointSize, glm::vec4 color, bool alwaysFront) {
    GizmoObject gizmo;

    gizmo.VBO = pointVBO;
    gizmo.VAO = pointVAO;
    gizmo.elements = 1;
    gizmo.primitive = GL_POINTS;

    gizmo.world = glm::mat4(1.0f);
    gizmo.world = glm::translate(gizmo.world, position);

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;
    gizmo.pointSize = pointSize;

    gizmoInstances.push_back(gizmo);
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

            vertices.push_back(x * zr0 * 0.5f);
            vertices.push_back(y * zr0 * 0.5f);
            vertices.push_back(z0 * 0.5f);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x * zr1 * 0.5f);
            vertices.push_back(y * zr1 * 0.5f);
            vertices.push_back(z1 * 0.5f);
            indices.push_back(indicator);
            indicator++;
        }
    }

    sphereIndicesCount = indices.size();
}
