#include "UI/MessageBox.h"

#include "Rendering/Assets/MaterialAsset.h"

#include "UI/Builders/ButtonBuilder.h"
#include "UI/Builders/LabelBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"

#include "UI/Components/UIComponent.h"
#include "UI/UIStyle.h"
#include <memory>

MessageBox::MessageBox(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

std::shared_ptr<MessageBox> MessageBox::Create(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent) {
    std::shared_ptr<MessageBox> messageBox = std::shared_ptr<MessageBox>(
            new MessageBox(id, name, isStatic, parent));

    std::shared_ptr<mlg::MaterialAsset> backgroundMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);

    std::shared_ptr<mlg::Image> background =
            messageBox->AddComponent<mlg::Image>("background", backgroundMaterial)
                    .lock();
    background->SetSize(MESSAGE_BOX_SIZE);

    mlg::LabelBuilder labelBuilder;
    messageBox->titleLabel = labelBuilder.SetName("title")
                                     .SetText("Title")
                                     .SetSize(30.f)
                                     .SetHorizontalAlignment(MLG_H_ALIGN_CENTER)
                                     .BuildLabel(messageBox.get())
                                     .lock();
    messageBox->titleLabel->SetRelativePosition(
            {0.f, MESSAGE_BOX_SIZE.y / 2.f - 40.0f});

    messageBox->textLabel = labelBuilder.SetName("message")
                                    .SetText("Message")
                                    .SetSize(20.f)
                                    .SetHorizontalAlignment(MLG_H_ALIGN_CENTER)
                                    .BuildLabel(messageBox.get())
                                    .lock();
    messageBox->textLabel->SetRelativePosition(
            {0.f, MESSAGE_BOX_SIZE.y / 2.0f - 80.f});

    mlg::ButtonBuilder buttonBuilder;
    std::shared_ptr<mlg::Button> okButton =
            buttonBuilder.SetName("OkButton")
                    .SetText("Ok")
                    .BuildButton(messageBox.get())
                    .lock();
    okButton->SetRelativePosition({0.f, -MESSAGE_BOX_SIZE.y / 2.0f + 50.f});

    okButton->OnClick.append(
            [messageBox]() {
                messageBox->panel->SetVisible(false);
                messageBox->OnMessageBoxHide();
            });

    messageBox->panel = messageBox->AddComponent<mlg::CanvasPanel>("Panel").lock();
    messageBox->panel->SetPosition(MLG_POS_CENTER);
    messageBox->panel->SetSize(MESSAGE_BOX_SIZE);
    messageBox->panel->AddChild(background);
    messageBox->panel->AddChild(messageBox->titleLabel);
    messageBox->panel->AddChild(messageBox->textLabel);
    messageBox->panel->AddChild(okButton);

    messageBox->panel->SetVisible(false);

    return messageBox;
}

MessageBox::~MessageBox() = default;

void MessageBox::ShowMessage(const std::string& title, const std::string& message) {
    panel->SetVisible(true);
    panel->GrabFocus();

    titleLabel->SetText(title);

    std::string wrappedMessage = mlg::Label::WrapText(message, 40);
    textLabel->SetText(wrappedMessage);

    OnMessageBoxShow();
}
