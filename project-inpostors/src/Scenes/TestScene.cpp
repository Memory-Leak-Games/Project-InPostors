#include "Scenes/TestScene.h"

#include "Buildings/Factory.h"
#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/EntityManager.h"

#include "Levels/NavigationGraph.h"
#include "SceneGraph/SceneGraph.h"

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") {}

void TestScene::Load() {
    LevelScene::Load();

    //navigationGraph = std::make_shared<NavigationGraph>("res/levels/maps/test_level.json");
}

void TestScene::Update() {
#ifdef DEBUG
    if (!mlg::SettingsManager::Get<bool>(
                mlg::SettingsType::Debug, "showNavigation"))
        return;

    if (navigationGraph == nullptr)
        return;

    //navigationGraph->DrawNodes();
#endif
}
