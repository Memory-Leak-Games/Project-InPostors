#pragma once

#include "Core/AssetManager/Asset.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace mlg {

    class SoundAsset : public Asset {
        uint32_t soundID = 0;
    public:
        explicit SoundAsset(const std::string& path);

        void Load() override;
        ~SoundAsset() override;

    private:
        int32_t ConvertToInt(char* buffer, std::size_t len);
        bool LoadWAVFileHeader(std::ifstream& file,
                               uint8_t& channels,
                               int32_t& sampleRate,
                               uint8_t& bitsPerSample,
                               ALsizei& size);
    };

}// namespace mlg
