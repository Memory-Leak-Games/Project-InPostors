#pragma once

#include <cstddef>
#include <string>

#include "glm/gtx/hash.hpp"

namespace mlg {
    struct Vertex;

    class Model {
    private:
        uint32_t vao;
        uint32_t vbo;
        uint32_t ebo;

        int32_t indiciesCount;

    public:
        explicit Model(const std::string& Path);

        void Draw();

    private:
        void SetupBuffers(
                const std::vector<Vertex>& vertices,
                const std::vector<int>& indices);

        void LoadObj(const std::string& Path);
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        bool operator==(const Vertex& other) const {
            return position == other.position &&
                   normal == other.normal &&
                   uv == other.uv;
        }
    };

}// namespace mlg

namespace std {
    template<>
    struct hash<mlg::Vertex> {
        size_t operator()(const mlg::Vertex& vertex) const {
            size_t seed = hash<glm::vec3>()(vertex.position);
            seed ^= (hash<glm::vec3>()(vertex.normal) << 1) >> 1;
            seed ^= (hash<glm::vec2>()(vertex.uv) << 1);

            return seed;
        }
    };
}// namespace std