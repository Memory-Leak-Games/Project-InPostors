#include "include/UI/Components/Label.h"

#include "UI/Components/UIComponent.h"
#include "glad/glad.h"

#include <glm/fwd.hpp>
#include <utility>

#include "Core/AssetManager/AssetManager.h"
#include "UI/Assets/FontAsset.h"

#include "Core/Time.h"
#include "Rendering/Assets/ShaderAsset.h"
#include "Rendering/ShaderProgram.h"
#include "UI/UIRenderer.h"

namespace mlg {

    Label::Label(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<class FontAsset>& font)
        : UIComponent(std::move(owner), std::move(name)), font(font), scale(32.f / font->fontSize) {
        shader = std::make_shared<ShaderProgram>(
                AssetManager::GetAsset<ShaderAsset>("res/shaders/ui/glyph.vert"),
                AssetManager::GetAsset<ShaderAsset>("res/shaders/ui/glyph.frag"));
    }

    void Label::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw Label");
        if(!visible)
            return;
        UIComponent::Draw(renderer);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Don't write into the depth buffer

        // Activate corresponding render state
        shader->Activate();
        shader->SetVec3F("textColor", textColor);
        shader->SetMat4F("projection", renderer->GetProjection());
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(renderer->vao);

        float actualScale = scale * renderer->uiScale;

        FontAsset::Character ch;
        float xpos, ypos, w, h;

        // Iterate through all characters
        glm::vec2 cursor = actualPosition;
        for (char8_t c : text) {

            // Skip rendering space
            if (c == ' ') {
                ZoneScopedN("Space");
                cursor.x += (font->fontSize >> 1) * actualScale;
            } else if (c == '\n') {
                cursor.x = actualPosition.x;
                cursor.y -= font->fontSize * actualScale;
            } else {
                ZoneScopedN("Char");
                ch = font->characters[c - 33];

                // Calculate position and size
                xpos = cursor.x + ch.Bearing.x * actualScale;
                ypos = ((int) cursor.y - (float) (ch.Size.y - ch.Bearing.y) * actualScale);

                w = (float) ch.Size.x * actualScale;
                h = (float) ch.Size.y * actualScale;

                shader->SetVec2F("pos", {xpos, ypos});
                shader->SetVec2F("size", {w, h});

                // Render glyph texture over quad
                glBindTextureUnit(0, ch.textureID);

                // Render quad
                {
                    ZoneScopedN("Draw call");
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                cursor.x += (ch.Advance >> 6) * actualScale;// Bitshift by 6 to get value in pixels (2^6 = 64)
            }
        }
        glBindVertexArray(0);
        glBindTextureUnit(0, 0);

        glDepthMask(GL_TRUE); // Re-enable writing to the depth buffer
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
        actualPositionDirty = true;
    }

    void Label::SetTextColor(const glm::vec3& textColor) {
        Label::textColor = textColor;
        actualPositionDirty = true;
    }

    void Label::SetSize(float size) {
        Label::scale = size / font->fontSize;
        actualPositionDirty = true;
    }

    void Label::SetHorizontalAlignment(Label::HorizontalAlignment alignment) {
        Label::horizontalAlignment = alignment;
        actualPositionDirty = true;
    }

    void Label::SetVerticalAlignment(VerticalAlignment alignment) {
        Label::verticalAlignment = alignment;
        actualPositionDirty = true;
    }

    void Label::CalculateActualPosition(const UIRenderer* renderer, const glm::vec2& position) {
        glm::vec2 alignedPosition = position;

        glm::vec2 labelSize = GetLabelSize();

        if (horizontalAlignment == HorizontalAlignment::Center)
            alignedPosition -= glm::vec2(labelSize.x / 2.f, 0.f);
        else if (horizontalAlignment == HorizontalAlignment::Right)
            alignedPosition -= glm::vec2(labelSize.x, 0.f);

        if (verticalAlignment == VerticalAlignment::Center)
            alignedPosition += glm::vec2(0.f, labelSize.y / 2.f);
        else if (verticalAlignment == VerticalAlignment::Up)
            alignedPosition += glm::vec2(0.f, labelSize.y);

        UIComponent::CalculateActualPosition(renderer, alignedPosition);
    }

    glm::vec2 Label::GetLabelSize() const {
        glm::vec2 result {0.f};

        // Calculate size of label
        float maxHorizontal = 0.f;

        for (char c : text) {
            if (c == '\n') {
                result.y += (float) font->fontSize * scale;
                maxHorizontal = std::max(maxHorizontal, result.x);
                continue;
            }

            result.x += (float) (font->characters[c - 33].Advance >> 6) * scale;    
        }

        result.x = std::max(maxHorizontal, result.x);
        result.y -= (float) font->fontSize * scale * 0.5f;

        return result;
    }
}

