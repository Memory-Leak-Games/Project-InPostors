#include "UI/Label.h"

#include <glad/glad.h>

#include "Core/Time.h"
#include "Core/Window.h"

#include "Core/AssetManager/AssetManager.h"
#include "UI/Assets/FontAsset.h"

#include "Rendering/Assets/ShaderAsset.h"
#include "Rendering/ShaderProgram.h"
#include "UI/Renderer2D.h"

mlg::Label::Label() {
    shader = std::make_shared<ShaderProgram>(
            AssetManager::GetAsset<ShaderAsset>("res/shaders/UI/glyph.vert"),
            AssetManager::GetAsset<ShaderAsset>("res/shaders/UI/glyph.frag"));

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 4 * sizeof(float));
}

void mlg::Label::Draw(const Renderer2D* renderer) {

    //TODO: Remove me later
    text = std::to_string(renderer->GetProjection()[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Activate corresponding render state
    shader->Activate();
    shader->SetVec3F("textColor", textColor);
    shader->SetMat4F("projection", renderer->GetProjection());
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // Iterate through all characters
    std::string::const_iterator c;
    GLfloat cursor = position.x;
    for (char8_t c : text) {
        FontAsset::Character ch = font->characters[c];

        float xpos = cursor + ch.Bearing.x;
        float ypos = position.y - (float) (ch.Size.y - ch.Bearing.y);

        float w = (float) ch.Size.x;
        float h = (float) ch.Size.y;
        // Update vbo for each character
        float vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };

        // Render glyph texture over quad
        glBindTextureUnit(0, ch.textureID);
        // Update content of vbo memory
        glNamedBufferSubData(vbo, 0, sizeof(vertices), vertices);

        // Render quad
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        cursor += (ch.Advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
}

