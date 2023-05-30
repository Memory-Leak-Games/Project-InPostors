#include "Scenes/TestScene.h"

#include "Gameplay/EntityManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Buildings/Factory.h"

#include "SceneGraph/SceneGraph.h"

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") { }

void TestScene::Load() {
    LevelScene::Load();
/*
    auto testFactory = mlg::EntityManager::SpawnEntity<Factory>("Smelter", false, mlg::SceneGraph::GetRoot(),
                                                                "res/levels/factories/smelter.json");
    auto testFactoryRigidBody = testFactory.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
    testFactoryRigidBody.lock()->SetPosition({22.f, 8.f});

    auto testMine = mlg::EntityManager::SpawnEntity<Factory>("Mine", false, mlg::SceneGraph::GetRoot(),
                                                                "res/levels/factories/mine.json");
    auto testMineRigidBody = testMine.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
    testMineRigidBody.lock()->SetPosition({-60.f, -5.f});
    testMineRigidBody.lock()->SetRotation(glm::radians(-90.f));

    auto testIkea = mlg::EntityManager::SpawnEntity<Factory>("Szwedzki sklep z meblami", false, mlg::SceneGraph::GetRoot(),
                                                                "res/levels/factories/ikea.json");
    auto testIkeaRigidBody = testIkea.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
    testIkeaRigidBody.lock()->SetPosition({55.f, -5.f});
    testIkeaRigidBody.lock()->SetRotation(glm::radians(-90.f));*/
}
