#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

//TODO: Rethink handling OpenAL errors
#define alCall(function, ...)  alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace mlg {

    class AudioAPI {
        static AudioAPI* instance;
        static ALCdevice* device;
        static ALCcontext* context;

        AudioAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        void Setup();

        static AudioAPI* GetInstance();
    private:
        static void OpenALErrorCheck(const std::string& filename, const std::uint_fast32_t line);
        static void OpenALCErrorCheck(const std::string& filename, const std::uint_fast32_t line);

        template<typename alFunction, typename... Params>
        auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
                ->typename std::enable_if<std::is_same<void,
                decltype(function(params...))>::value,
                decltype(function(params...))>::type {
            function(std::forward<Params>(params)...);
            OpenALErrorCheck(filename, line);
        }

        template<typename alcFunction, typename... Params>
        auto alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, Params... params)
                ->typename std::enable_if<std::is_same<void,
                decltype(function(params...))>::value,
                decltype(function(params...))>::type {
            function(std::forward<Params>(params)...);
            OpenALCErrorCheck(filename, line);
        }

        template<typename alFunction, typename... Params>
        auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
                ->typename std::enable_if<!std::is_same<void,
                decltype(function(params...))>::value,
                decltype(function(params...))>::type {
            auto ret = function(std::forward<Params>(params)...);
            OpenALErrorCheck(filename, line);
            return ret;
        }

        template<typename alcFunction, typename... Params>
        auto alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, Params... params)
                ->typename std::enable_if<!std::is_same<void,
                decltype(function(params...))>::value,
                decltype(function(params...))>::type {
            auto ret = function(std::forward<Params>(params)...);
            OpenALCErrorCheck(filename, line);
            return ret;
        }
    };

}// namespace mlg
