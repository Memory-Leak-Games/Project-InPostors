#include "Scenes/TestScene.h"

#include "Buildings/Factory.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/RGBA.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/EntityManager.h"

#include "Levels/NavigationGraph.h"
#include "SceneGraph/SceneGraph.h"

#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"
#include "ai/Path.h"

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") {}

void TestScene::Load() {
    LevelScene::Load();

    navigationGraph = std::make_shared<NavigationGraph>("res/levels/maps/test_level.json");

    //TODO: Maybe pass the entire graph into steering behaviours and let them decide (maybe randomly) which of the connected nodes should be next

    auto testCarOnePath = navigationGraph->CreatePath();

    TrafficCarData testCarOneData = {0, mlg::RGBA::white};
    auto testCarOne = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar1", false, mlg::SceneGraph::GetRoot(), testCarOneData);
    testCarOne.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->SetPath(testCarOnePath);
    testCarOne.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->TrafficDriveOn();
    auto testCarOneRigidbody = testCarOne.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
    testCarOneRigidbody.lock()->SetPosition(*testCarOnePath.begin());

}

void TestScene::Update() {
#ifdef DEBUG
    if (!mlg::SettingsManager::Get<bool>(
                mlg::SettingsType::Debug, "showNavigation"))
        return;

    if (navigationGraph == nullptr)
        return;

    navigationGraph->DrawNodes();
#endif
}
