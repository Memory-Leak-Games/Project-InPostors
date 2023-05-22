#pragma once

#include "Neighbours.h"

namespace mlg {

    class LevelGenerator {

    public:
        static std::vector<std::string> LoadMap(const std::string& path);
        static void LoadCameraSettings(const std::string& path, class CameraComponent& cameraComponent);
        static void SpawnGround(const std::string& path);
        static void SetCityBounds(const std::string& path);


    private:
        struct MapObject {
            std::shared_ptr<class ModelAsset> model;
            std::shared_ptr<class MaterialAsset> material;

            float worldRot;
            float scale;

            bool hasCollision = false;
            std::string colliderType;
            float colliderSize = 1.0f;
            float colliderOffset = 0.0f;
        };

        struct Roads {
            char symbol = ' ';
            MapObject road;
            MapObject edge;
            MapObject cross;
            MapObject curve;
            MapObject corner;
        } roadsObjects;

        struct MapEntry {
            std::vector<MapObject> objectsPool;
            bool isPathWay = false;

            int useCount = 0;
        };

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        nlohmann::json levelJson;
        nlohmann::json tileJson;

        std::vector<std::string> levelLayout;

        std::string ignoredCharacters;
        std::unordered_map<char, MapEntry> mapObjects;
        std::string defaultMaterial;

        float tileSize = 10.f;
        glm::vec2 citySize = {0.f, 0.f};

        std::vector<std::string> LoadLayout();
        void LoadMapObjects();
        void LoadFactories(); //todo
        void LoadRoads();
        MapObject ParseObject(const nlohmann::json& jsonObject);

        void GenerateLevel();

        void PutTile(int x, int y, const char& character);
        void PutRoad(int x, int y);

        void PutStraightRoad(int x, int y, bool isVertical);
        void PutEdgeRoad(int x, int y);
        void PutCrossRoad(int x, int y);
        void PutCurveRoad(int x, int y);
        void PutCornerRoad(int x, int y);

        void PutEntity(const MapObject& mapObject, const glm::ivec2& position, float rotation) const;
        float GetSmartRotation(int x, int y);

        glm::vec2 GetCitySize();
        Neighbours GetNeighbours(int x, int y);
        char GetTileOrZero(int x, int y);

        glm::ivec2 GetLayoutSize();
    };

} //mlg
 