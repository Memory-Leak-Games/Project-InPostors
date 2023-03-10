#include "Skybox.h"

#include <string>
#include <utility>

#include "LoggingMacros.h"
#include "stb_image.h"
#include "ShaderWrapper.h"

Skybox::Skybox(const std::array<std::string, 6>& cubeTextures, std::shared_ptr<ShaderWrapper> shader)
: shader(std::move(shader)) {
    InitializeBuffers();
    LoadCubeMap(cubeTextures);

}

void Skybox::InitializeBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Skybox::LoadCubeMap(const std::array<std::string, 6>& cubeTextures) {
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < cubeTextures.size(); i++) {
        SPDLOG_DEBUG("Loading cubemap texture at path: {}", cubeTextures[i]);

        unsigned char* data = stbi_load(cubeTextures[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            SPDLOG_ERROR("Cubemap texure failed to load at path: {}", cubeTextures[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Skybox::textureId = textureId;
}

void Skybox::Draw() {
    glDepthFunc(GL_LEQUAL);
    shader->Activate();
    shader->SetInt("cubemap", 0);

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

unsigned int Skybox::GetTextureId() const {
    return textureId;
}
