#include "Initializer.h"

#include "Core/Core.h"

#include "Buildings/Factory.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/TimerManager.h"

#include "Levels/LevelGenerator.h"
#include "SceneGraph/SceneGraph.h"

#include "Gameplay/EntityManager.h"
#include "Gameplay/Components/CameraComponent.h"
#include <Gameplay/Components/RigidbodyComponent.h>
#include <Gameplay/Components/StaticMeshComponent.h>

class ProjectInpostors {
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Initializer::InitializeCoreComponents();
        mlg::Initializer::InitializeSceneComponents();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::Initializer::StopSceneComponents();
        mlg::Initializer::InitializeSceneComponents();
        PrepareScene();
        engine->MainLoop();

        mlg::Initializer::StopSceneComponents();
        mlg::Initializer::StopCoreComponents();

        return 0;
    }

    void PrepareScene() {
        auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
        auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

        std::vector<std::string> levelLayout;

        const std::string testLevelPath = "res/levels/maps/test_level.json";

        levelLayout = mlg::LevelGenerator::LoadMap(testLevelPath);
        mlg::LevelGenerator::SpawnGround(testLevelPath);
        mlg::LevelGenerator::SetCityBounds(testLevelPath);
        mlg::LevelGenerator::LoadCameraSettings(testLevelPath, *cameraComponent.lock());
        mlg::LevelGenerator::SpawnPlayers(testLevelPath);


        // create factories
        //auto testFactory = mlg::EntityManager::SpawnEntity<Factory>("Smelter", false, mlg::SceneGraph::GetRoot(),
        //                                                          "res/levels/factories/smelter.json");
        //auto testFactoryRigidBody = testFactory.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        //testFactoryRigidBody.lock()->SetPosition({22.f, 8.f});

        auto testMine = mlg::EntityManager::SpawnEntity<Factory>("Mine", false, mlg::SceneGraph::GetRoot(),
                                                                 "res/levels/factories/mine.json");
        auto testMineRigidBody = testMine.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testMineRigidBody.lock()->SetPosition({-60.f, -5.f});
        testMineRigidBody.lock()->SetRotation(glm::radians(-90.f));

        auto testIkea = mlg::EntityManager::SpawnEntity<Factory>("Szwedzki sklep z meblami", false, mlg::SceneGraph::GetRoot(),
                                                                 "res/levels/factories/ikea.json");
        auto testIkeaRigidBody = testIkea.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testIkeaRigidBody.lock()->SetPosition({55.f, -5.f});
        testIkeaRigidBody.lock()->SetRotation(glm::radians(-90.f));
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
