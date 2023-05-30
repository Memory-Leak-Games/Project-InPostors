#include "Rendering/Model.h"
#include "Core/Math.h"
#include <cassert>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Macros.h"

using namespace mlg;

void Model::Draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indiciesCount, GL_UNSIGNED_INT, nullptr);
}

Model::Model(const std::string& path) {
    LoadObj(path);
}

void mlg::Model::SetupBuffers(
        const std::vector<Vertex>& vertices, const std::vector<int>& indices) {
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glNamedBufferStorage(vbo, vertices.size() * sizeof(Vertex), &vertices[0], 0);
    glNamedBufferStorage(ebo, indices.size() * sizeof(uint32_t), &indices[0], 0);

    // Positions
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

    // normals
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

    // UV's
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao, ebo);
}

void mlg::Model::LoadObj(const std::string& path) {
    tinyobj::ObjReaderConfig objReaderConfig;
    objReaderConfig.vertex_color = false; 

    tinyobj::ObjReader objReader;

    bool result = objReader.ParseFromFile(path, objReaderConfig);
    MLG_ASSERT_MSG(result, "TinyObj error: " + objReader.Error());

    if (!objReader.Warning().empty())
        SPDLOG_WARN("TinyObj: {}", objReader.Warning());

    std::vector<Vertex> vertices;
    std::vector<int> indices;

    const tinyobj::attrib_t& attrib = objReader.GetAttrib();

    std::unordered_map<Vertex, uint32_t> visitedVertices;

    for (const auto& shape : objReader.GetShapes()) {
        for (const auto& index : shape.mesh.indices) {


            Vertex vertex{};
            vertex.position = {
                    attrib.vertices[3 * size_t(index.vertex_index) + 0],
                    attrib.vertices[3 * size_t(index.vertex_index) + 1],
                    attrib.vertices[3 * size_t(index.vertex_index) + 2]};

            vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]};

            vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};

            vertex.normal = Math::SafeNormal(vertex.normal);

            if (!visitedVertices.contains(vertex)) {
                visitedVertices.insert({vertex, vertices.size()});
                vertices.push_back(vertex);
            }

            indices.push_back(visitedVertices[vertex]);
        }

        indiciesCount = indices.size();
        SetupBuffers(vertices, indices);
    }
}
