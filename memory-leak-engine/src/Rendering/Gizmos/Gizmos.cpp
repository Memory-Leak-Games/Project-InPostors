#include "Rendering/Gizmos/Gizmos.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/Time.h"
#include "Rendering/Assets/ShaderAsset.h"
#include "Rendering/ShaderProgram.h"
#include "Core/RGBA.h"

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
glm::vec4 Gizmos::defaultColor = RGBA::red;

std::vector<Gizmos::GizmoObject> Gizmos::gizmoInstances;

float lineVertices[]{
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f};

float boxVertices[]{
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
};

uint32_t boxIndices[]{
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
};

// Yes, I also find this silly
float pointVertex[]{0.0f, 0.0f, 0.0f};

void mlg::Gizmos::Initialize() {
    // Load gizmo shader
    shader = new ShaderProgram(
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.vert"),
            mlg::AssetManager::GetAsset<ShaderAsset>("res/shaders/Utils/gizmo.frag"));

    // Generate line gizmo
    {
        glCreateVertexArrays(1, &lineVAO);

        glCreateBuffers(1, &lineVBO);
        glNamedBufferData(lineVAO, 2 * sizeof(glm::vec3), lineVertices, GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(lineVAO, 0);
        glVertexArrayAttribBinding(lineVAO, 0, 0);
        glVertexArrayAttribFormat(lineVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(lineVAO, 0, lineVBO, 0, sizeof(glm::vec3));
    }

    // Generate box gizmo
    {
        glCreateVertexArrays(1, &boxVAO);

        glCreateBuffers(1, &boxVBO);
        glCreateBuffers(1, &boxEBO);
        glNamedBufferData(boxVBO, 8 * sizeof(glm::vec3), boxVertices, GL_STATIC_DRAW);
        glNamedBufferData(boxEBO, 24 * sizeof(float), boxIndices, GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(boxVAO, 0);
        glVertexArrayAttribBinding(boxVAO, 0, 0);
        glVertexArrayAttribFormat(boxVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(boxVAO, 0, boxVBO, 0, sizeof(glm::vec3));
        glVertexArrayElementBuffer(boxVAO, boxEBO);
    }

    // Generate sphere gizmo
    {
        std::vector<GLfloat> SphereVertices;
        std::vector<GLuint> SphereIndices;
        GenerateSphere(SphereVertices, SphereIndices);

        glCreateVertexArrays(1, &sphereVAO);

        glCreateBuffers(1, &sphereVBO);
        glCreateBuffers(1, &sphereEBO);
        glNamedBufferData(sphereVBO, SphereVertices.size() * sizeof(glm::vec3), &SphereVertices[0], GL_STATIC_DRAW);
        glNamedBufferData(sphereEBO, SphereIndices.size() * sizeof(float), &SphereIndices[0], GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(sphereVAO, 0);
        glVertexArrayAttribBinding(sphereVAO, 0, 0);
        glVertexArrayAttribFormat(sphereVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(sphereVAO, 0, sphereVBO, 0, sizeof(glm::vec3));
        glVertexArrayElementBuffer(sphereVAO, sphereEBO);
    }

    // Generate point gizmo
    {
        glCreateVertexArrays(1, &pointVAO);

        glCreateBuffers(1, &pointVBO);
        glNamedBufferData(pointVBO, sizeof(glm::vec3), lineVertices, GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(pointVAO, 0);
        glVertexArrayAttribBinding(pointVAO, 0, 0);
        glVertexArrayAttribFormat(pointVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(pointVAO, 0, pointVBO, 0, sizeof(glm::vec3));
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
    ZoneScoped;

    glEnable(GL_DEPTH_TEST);
    shader->Activate();
    glEnable(GL_PROGRAM_POINT_SIZE);
    auto gizmoEnd = gizmoInstances.end();
    for (auto gizmoIt = gizmoInstances.begin(); gizmoIt < gizmoEnd;) {
        //GizmoObject gizmo = *gizmoIt;
        shader->SetMat4F("world", gizmoIt->world);
        shader->SetVec4F("color", gizmoIt->color);
        shader->SetBool("alwaysFront", gizmoIt->alwaysFront);
        shader->SetFloat("pointSize", gizmoIt->pointSize);

        glBindVertexArray(gizmoIt->VAO);
        if (gizmoIt->EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmoIt->EBO);
            glDrawElements(gizmoIt->primitive, gizmoIt->elements, GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(gizmoIt->primitive, 0, gizmoIt->elements);
        }

        // Erase outdated gizmos without causing segmentation fault
        gizmoIt->ttl -= Time::GetTrueDeltaSeconds();
        if(gizmoIt->ttl <= 0.0f)
        {
            gizmoInstances.erase(gizmoIt);
            gizmoEnd--;
        }
        else
        {
            gizmoIt++;
        }
    }

    glDisable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mlg::Gizmos::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, bool alwaysFront, float ttl) {
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
    gizmo.ttl = ttl;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawBox(glm::vec3 position, glm::vec3 size, glm::quat rotation, glm::vec4 color, bool alwaysFront, float ttl) {
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
    gizmo.ttl = ttl;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawBox(Transform& transform, glm::vec4 color, bool alwaysFront, float ttl) {
    GizmoObject gizmo;

    gizmo.VBO = boxVBO;
    gizmo.VAO = boxVAO;
    gizmo.EBO = boxEBO;
    gizmo.elements = 24;

    gizmo.world = transform.GetLocalMatrix();

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;
    gizmo.ttl = ttl;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawSphere(glm::vec3 position, float radius, glm::vec4 color, bool alwaysFront, float ttl) {
    GizmoObject gizmo;

    gizmo.VBO = sphereVBO;
    gizmo.VAO = sphereVAO;
    gizmo.EBO = sphereEBO;
    gizmo.elements = sphereIndicesCount;
    gizmo.primitive = GL_LINE_STRIP;

    gizmo.world = glm::mat4(1.0f);
    gizmo.world = glm::translate(gizmo.world, position);
    gizmo.world = glm::scale(gizmo.world, glm::vec3(radius * 2)); // Default radius is 1

    gizmo.color = color;
    gizmo.alwaysFront = alwaysFront;
    gizmo.ttl = ttl;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::DrawPoint(glm::vec3 position, glm::vec4 color, bool alwaysFront, float ttl) {
    DrawSizedPoint(position, 2, color, alwaysFront, ttl);
}

void Gizmos::DrawSizedPoint(glm::vec3 position, float pointSize, glm::vec4 color, bool alwaysFront, float ttl) {
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
    gizmo.ttl = ttl;

    gizmoInstances.push_back(gizmo);
}

void Gizmos::GenerateSphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
    constexpr float radius = 0.5f;
    constexpr uint32_t LOD = 8;
    uint32_t indicator = 0;

    for (uint32_t i = 0; i <= LOD; i++) {
        float lat0 = glm::pi<float>() * (-0.5f + (float) (i - 1) / (float) LOD);
        float z0 = glm::sin(lat0);
        float zr0 = glm::cos(lat0);

        float lat1 = glm::pi<float>() * (-0.5f + (float) i / (float) LOD);
        float z1 = glm::sin(lat1);
        float zr1 = glm::cos(lat1);

        for (uint32_t j = 0; j <= LOD; j++) {
            float lng = 2 * glm::pi<float>() * (float) (j - 1) / (float) LOD;
            float x = glm::cos(lng);
            float y = glm::sin(lng);

            vertices.push_back(x * zr0 * radius);
            vertices.push_back(y * zr0 * radius);
            vertices.push_back(z0 * radius);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x * zr1 * radius);
            vertices.push_back(y * zr1 * radius);
            vertices.push_back(z1 * radius);
            indices.push_back(indicator);
            indicator++;
        }
    }

    sphereIndicesCount = (int32_t) indices.size();
}
