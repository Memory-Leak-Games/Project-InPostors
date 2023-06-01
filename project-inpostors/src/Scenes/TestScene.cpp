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
}
