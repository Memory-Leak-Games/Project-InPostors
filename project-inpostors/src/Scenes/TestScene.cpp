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

    TrafficCarData testCarOneData = {0, mlg::RGBA::white};
    auto testCarOne = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar1", false, mlg::SceneGraph::GetRoot(), testCarOneData);
    testCarOne.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->SetNavigationGraph(navigationGraph);
    auto testCarOneRigidbody = testCarOne.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
    testCarOneRigidbody.lock()->SetPosition({5, 5});
    testCarOne.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->TrafficDriveOn();

    TrafficCarData testCarTwoData = {0, mlg::RGBA::white};
    auto testCarTwo = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar1", false, mlg::SceneGraph::GetRoot(), testCarOneData);
    testCarTwo.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->SetNavigationGraph(navigationGraph);
    auto testCarTwoRigidbody = testCarOne.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
    testCarTwoRigidbody.lock()->SetPosition({-5, -5});
    testCarTwo.lock()->GetComponentByName<AIComponent>("AIComponent").lock()->GetSteering()->TrafficDriveOn();

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
