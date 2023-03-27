#include "Rendering/Gizmos/Gizmos.h"

using namespace mlg;

GLuint Gizmos::LineVAO;
GLuint Gizmos::LineVBO;
GLuint Gizmos::BoxVAO;
GLuint Gizmos::BoxVBO;
GLuint Gizmos::BoxEBO;
GLuint Gizmos::SphereVAO;
GLuint Gizmos::SphereVBO;
GLuint Gizmos::SphereEBO;
GLuint Gizmos::SphereIndiciesCount;
GLuint Gizmos::PointVBO;
GLuint Gizmos::PointVAO;

std::shared_ptr<ShaderWrapper> Gizmos::Shader;
glm::vec4 Gizmos::DefaultColor = {0, 1, 0 ,1};

GLfloat LineVertices[]
{
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f
};

GLfloat BoxVertices[]
{
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
};

GLuint BoxIndices[]
{
    0, 1, 1, 2, 2, 3, 3, 0,
    4, 5, 5, 6, 6, 7, 7, 4,
    0, 4, 1, 5, 2, 6, 3, 7
};

// Yes, I also find this silly
GLfloat PointVertex[]
{
    0.0f, 0.0f, 0.0f
};

void mlg::Gizmos::Initialize()
{
    // Load gizmo shader
    Shader = std::make_shared<ShaderWrapper>("res/shaders/gizmo.vert", "res/shaders/gizmo.frag");

    // Generate line gizmo
    {
        glGenBuffers(1, &LineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), LineVertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &LineVAO);
        glBindVertexArray(LineVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate box gizmo
    {
        glGenBuffers(1, &BoxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, BoxVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), BoxVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &BoxEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), BoxIndices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &BoxVAO);
        glBindVertexArray(BoxVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLuint), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate sphere gizmo
    {
        std::vector<GLfloat> SphereVertices;
        std::vector<GLuint> SphereIndices;
        GenerateSphere(SphereVertices, SphereIndices, 30);

        glGenBuffers(1, &SphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), &SphereVertices[0], GL_STATIC_DRAW);
        glGenBuffers(1, &SphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), &SphereIndices[0], GL_STATIC_DRAW);
        glGenVertexArrays(1, &SphereVAO);
        glBindVertexArray(SphereVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLuint), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate point gizmo
    {
        glGenBuffers(1, &PointVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PointVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), PointVertex, GL_STATIC_DRAW);
        glGenVertexArrays(1, &PointVAO);
        glBindVertexArray(PointVAO);
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

void mlg::Gizmos::Stop()
{

}

void mlg::Gizmos::DrawLine(glm::vec3 Start, glm::vec3 End, glm::vec4 Color, bool AlwaysFront)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Start);
    World = glm::scale(World, End - Start); // Use position diff to scale line (which is from [0, 0, 0] to [1, 1, 1])

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);
    Shader->SetBool("AlwaysFront", AlwaysFront);

    glBindVertexArray(LineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(Transform& Transform, glm::vec4 Color, bool AlwaysFront)
{
    const glm::mat4 World = Transform.GetLocalMatrix();

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);
    Shader->SetBool("AlwaysFront", AlwaysFront);

    glBindVertexArray(BoxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(glm::vec3 Position, glm::vec3 Size, glm::quat Rotation, glm::vec4 Color, bool AlwaysFront)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Position);
    World = World * glm::toMat4(Rotation);
    World = glm::scale(World, Size); // Default size is 1x1x1

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);
    Shader->SetBool("AlwaysFront", AlwaysFront);

    glBindVertexArray(BoxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawSphere(glm::vec3 Position, float Radius, glm::vec4 Color, bool AlwaysFront)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Position);
    World = glm::scale(World, glm::vec3(Radius * 2)); // Default radius is 1

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);
    Shader->SetBool("AlwaysFront", AlwaysFront);

    glBindVertexArray(SphereVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glDrawElements(GL_LINES, SphereIndiciesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawPoint(glm::vec3 Position, glm::vec4 Color, bool AlwaysFront)
{
    DrawPoint(Position, 2, Color, AlwaysFront);
}

void Gizmos::DrawPoint(glm::vec3 Position, float Size, glm::vec4 Color, bool AlwaysFront)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Position);

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);
    Shader->SetBool("AlwaysFront", AlwaysFront);
    Shader->SetFloat("PointSize", Size);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(PointVAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

// FIXME: It's not a sphere :/
void Gizmos::GenerateSphere(std::vector<GLfloat>& Vertices, std::vector<GLuint>& Indices, int LOD)
{
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

            Vertices.push_back(x * zr0 * 0.5);
            Vertices.push_back(y * zr0 * 0.5);
            Vertices.push_back(z0 * 0.5);
            Indices.push_back(indicator);
            indicator++;

            Vertices.push_back(x * zr1 * 0.5);
            Vertices.push_back(y * zr1 * 0.5);
            Vertices.push_back(z1 * 0.5);
            Indices.push_back(indicator);
            indicator++;
        }
    }

    SphereIndiciesCount = Indices.size();
}
