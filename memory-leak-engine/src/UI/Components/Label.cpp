#include "include/UI/Components/Label.h"

#include "glad/glad.h"

#include <utility>

#include "Core/AssetManager/AssetManager.h"
#include "UI/Assets/FontAsset.h"

#include "Rendering/Assets/ShaderAsset.h"
#include "Rendering/ShaderProgram.h"
#include "UI/Renderer2D.h"

namespace mlg {

    Label::Label(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<class FontAsset>& font)
        : UIComponent(std::move(owner), std::move(name)), font(font) {
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

    void Label::Draw(const Renderer2D* renderer) {

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

        float actualScale = renderer->uiScale * scale;

        // Iterate through all characters
        float cursor = position.x * actualScale;
        for (char8_t c : text) {
            FontAsset::Character ch = font->characters[c];

            float xpos = cursor + ch.Bearing.x * actualScale;
            float ypos = (position.y - (float) (ch.Size.y - ch.Bearing.y)) * actualScale;

            float w = (float) ch.Size.x * actualScale;
            float h = (float) ch.Size.y * actualScale;
            // Update vbo for each character
            float vertices[6][4] = {
                    {xpos, ypos + h, 0.0, 0.0},
                    {xpos, ypos, 0.0, 1.0},
                    {xpos + w, ypos, 1.0, 1.0},

                    {xpos, ypos + h, 0.0, 0.0},
                    {xpos + w, ypos, 1.0, 1.0},
                    {xpos + w, ypos + h, 1.0, 0.0}};

            // Render glyph texture over quad
            glBindTextureUnit(0, ch.textureID);
            // Update content of vbo memory
            glNamedBufferSubData(vbo, 0, sizeof(vertices), vertices);

            // Render quad
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            cursor += (ch.Advance >> 6) * actualScale;// Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTextureUnit(0, 0);
    }

    std::shared_ptr<FontAsset> Label::GetFont() const {
        return font;
    }

    const std::string& Label::GetText() const {
        return text;
    }

    const glm::vec3& Label::GetTextColor() const {
        return textColor;
    }

    void Label::SetFont(const std::shared_ptr<FontAsset>& font) {
        Label::font = font;
    }

    void Label::SetText(const std::string& text) {
        Label::text = text;
    }

    void Label::SetTextColor(const glm::vec3& textColor) {
        Label::textColor = textColor;
    }

    void Label::SetSize(float size) {
        Label::scale = size / font->fontSize;
    }
}

