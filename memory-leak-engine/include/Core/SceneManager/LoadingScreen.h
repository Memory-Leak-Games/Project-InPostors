#pragma once

namespace mlg {

    class LoadingScreen {
        std::unique_ptr<class QuadRenderer> backgroundQuad;
        std::unique_ptr<class Rect> loadingImage;

        std::shared_ptr<class MaterialAsset> backgroundMaterial;
        std::shared_ptr<class MaterialAsset> imageMaterial;

    public:
        LoadingScreen();
        ~LoadingScreen();

        void Draw();
    };

} // namespace mlg