#include "Audio/Assets/SoundAsset.h"

#include "Macros.h"

namespace mlg {
    SoundAsset::SoundAsset(const std::string& path) : Asset(path) {
    }

    void SoundAsset::Load() {
        std::string path = GetPath();
        SPDLOG_DEBUG("Loading sound at path: {}", path);

//        std::ifstream in(path, std::ios::binary);
    }

    SoundAsset::~SoundAsset() {
    }

    int32_t SoundAsset::ConvertToInt(char* buffer, std::size_t len) {
        int32_t res = 0;
        if (std::endian::native == std::endian::little)
            std::memcpy(&res, buffer, len);
        else
            for (std::size_t i = 0; i < len; ++i) {
                reinterpret_cast<char*>(&res)[3 - i] = buffer[i];
            }
        return res;
    }

    bool SoundAsset::LoadWAVFileHeader(std::ifstream& file,
                                       uint8_t& channels,
                                       int32_t& sampleRate,
                                       uint8_t& bitsPerSample,
                                       ALsizei& size) {
        char buffer[4];
        if (true)
            return false;

        // The RIFF


        return false;
    }
}// namespace mlg