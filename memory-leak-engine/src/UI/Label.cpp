#include "UI/Label.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/ShaderAsset.h"
#include "Rendering/ShaderProgram.h"
#include "UI/Assets/FontAsset.h"
#include "core/window.h"
#include "glad/glad.h"

mlg::Label::Label() {
    shader = std::make_shared<ShaderProgram>(
            AssetManager::GetAsset<ShaderAsset>("res/shaders/UI/glyph.vert"),
            AssetManager::GetAsset<ShaderAsset>("res/shaders/UI/glyph.frag"));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void mlg::Label::Draw(struct Renderer2D* renderer) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Window* window = Window::GetInstance();
    glm::mat4 projection = glm::ortho(0.0f, (float)window->GetWidth(), 0.0f, (float)window->GetHeight());

    // Activate corresponding render state
    shader->Activate();
    shader->SetVec3F("textColor", textColor);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    GLfloat cursor = position.x;
    for (c = text.begin(); c != text.end(); c++)
    {
        FontAsset::Character ch = font->characters[*c];

        float xpos = cursor + ch.Bearing.x * font->fontSize;
        float ypos = position.y - (ch.Size.y - ch.Bearing.y) * font->fontSize;

        float w = ch.Size.x * font->fontSize;
        float h = ch.Size.y * font->fontSize;
        // Update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        cursor += (ch.Advance >> 6) * font->fontSize; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

