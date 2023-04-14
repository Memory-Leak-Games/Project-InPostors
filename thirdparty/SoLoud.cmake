CPMAddPackage("gh:jarikomppa/soloud#RELEASE_20200207")

set(HEADER_PATH ${soloud_SOURCE_DIR}/include)
set(SOURCE_PATH ${soloud_SOURCE_DIR}/src)

# Headers
set(TARGET_HEADERS
        ${HEADER_PATH}/soloud.h
        ${HEADER_PATH}/soloud_audiosource.h
        ${HEADER_PATH}/soloud_bassboostfilter.h
        ${HEADER_PATH}/soloud_biquadresonantfilter.h
        ${HEADER_PATH}/soloud_bus.h
        ${HEADER_PATH}/soloud_c.h
        ${HEADER_PATH}/soloud_dcremovalfilter.h
        ${HEADER_PATH}/soloud_echofilter.h
        ${HEADER_PATH}/soloud_error.h
        ${HEADER_PATH}/soloud_fader.h
        ${HEADER_PATH}/soloud_fft.h
        ${HEADER_PATH}/soloud_fftfilter.h
        ${HEADER_PATH}/soloud_file.h
        ${HEADER_PATH}/soloud_file_hack_off.h
        ${HEADER_PATH}/soloud_file_hack_on.h
        ${HEADER_PATH}/soloud_filter.h
        ${HEADER_PATH}/soloud_flangerfilter.h
        ${HEADER_PATH}/soloud_internal.h
        ${HEADER_PATH}/soloud_lofifilter.h
        ${HEADER_PATH}/soloud_monotone.h
        ${HEADER_PATH}/soloud_openmpt.h
        ${HEADER_PATH}/soloud_queue.h
        ${HEADER_PATH}/soloud_robotizefilter.h
        ${HEADER_PATH}/soloud_sfxr.h
        ${HEADER_PATH}/soloud_speech.h
        ${HEADER_PATH}/soloud_tedsid.h
        ${HEADER_PATH}/soloud_thread.h
        ${HEADER_PATH}/soloud_vic.h
        ${HEADER_PATH}/soloud_vizsn.h
        ${HEADER_PATH}/soloud_wav.h
        ${HEADER_PATH}/soloud_waveshaperfilter.h
        ${HEADER_PATH}/soloud_wavstream.h
        )

# Core
set(CORE_PATH ${SOURCE_PATH}/core)
set(CORE_SOURCES
        ${CORE_PATH}/soloud.cpp
        ${CORE_PATH}/soloud_audiosource.cpp
        ${CORE_PATH}/soloud_bus.cpp
        ${CORE_PATH}/soloud_core_3d.cpp
        ${CORE_PATH}/soloud_core_basicops.cpp
        ${CORE_PATH}/soloud_core_faderops.cpp
        ${CORE_PATH}/soloud_core_filterops.cpp
        ${CORE_PATH}/soloud_core_getters.cpp
        ${CORE_PATH}/soloud_core_setters.cpp
        ${CORE_PATH}/soloud_core_voicegroup.cpp
        ${CORE_PATH}/soloud_core_voiceops.cpp
        ${CORE_PATH}/soloud_fader.cpp
        ${CORE_PATH}/soloud_fft.cpp
        ${CORE_PATH}/soloud_fft_lut.cpp
        ${CORE_PATH}/soloud_file.cpp
        ${CORE_PATH}/soloud_filter.cpp
        ${CORE_PATH}/soloud_queue.cpp
        ${CORE_PATH}/soloud_thread.cpp
        )


# Audiosources
set(AUDIOSOURCES_PATH ${SOURCE_PATH}/audiosource)
set(AUDIOSOURCES_SOURCES
        ${AUDIOSOURCES_PATH}/monotone/soloud_monotone.cpp
        ${AUDIOSOURCES_PATH}/openmpt/soloud_openmpt.cpp
        ${AUDIOSOURCES_PATH}/openmpt/soloud_openmpt_dll.c
        ${AUDIOSOURCES_PATH}/sfxr/soloud_sfxr.cpp
        ${AUDIOSOURCES_PATH}/speech/darray.cpp
        ${AUDIOSOURCES_PATH}/speech/klatt.cpp
        ${AUDIOSOURCES_PATH}/speech/resonator.cpp
        ${AUDIOSOURCES_PATH}/speech/soloud_speech.cpp
        ${AUDIOSOURCES_PATH}/speech/tts.cpp
        ${AUDIOSOURCES_PATH}/tedsid/sid.cpp
        ${AUDIOSOURCES_PATH}/tedsid/soloud_tedsid.cpp
        ${AUDIOSOURCES_PATH}/tedsid/ted.cpp
        ${AUDIOSOURCES_PATH}/vic/soloud_vic.cpp
        ${AUDIOSOURCES_PATH}/vizsn/soloud_vizsn.cpp
        ${AUDIOSOURCES_PATH}/wav/dr_impl.cpp
        ${AUDIOSOURCES_PATH}/wav/soloud_wav.cpp
        ${AUDIOSOURCES_PATH}/wav/soloud_wavstream.cpp
        ${AUDIOSOURCES_PATH}/wav/stb_vorbis.c
        )

# Filters
set(FILTERS_PATH ${SOURCE_PATH}/filter)
set(FILTERS_SOURCES
        ${FILTERS_PATH}/soloud_bassboostfilter.cpp
        ${FILTERS_PATH}/soloud_biquadresonantfilter.cpp
        ${FILTERS_PATH}/soloud_dcremovalfilter.cpp
        ${FILTERS_PATH}/soloud_echofilter.cpp
        ${FILTERS_PATH}/soloud_fftfilter.cpp
        ${FILTERS_PATH}/soloud_flangerfilter.cpp
        ${FILTERS_PATH}/soloud_lofifilter.cpp
        ${FILTERS_PATH}/soloud_robotizefilter.cpp
        ${FILTERS_PATH}/soloud_waveshaperfilter.cpp
        )


# All together
source_group("Includes" FILES ${TARGET_HEADERS})
source_group("Core" FILES ${CORE_SOURCES})
source_group("Audiosources" FILES ${AUDIOSOURCES_SOURCES})
source_group("Filters" FILES ${FILTERS_SOURCES})

if (UNIX)
    set(BACKENDS_SOURCES ${soloud_SOURCE_DIR}/src/backend/alsa/soloud_alsa.cpp)
elseif (WIN32)
    set(BACKENDS_SOURCES ${soloud_SOURCE_DIR}/src/backend/winmm/soloud_winmm.cpp)
endif ()

add_library(soloud
        ${CORE_SOURCES}
        ${AUDIOSOURCES_SOURCES}
        ${BACKENDS_SOURCES}
        ${FILTERS_SOURCES})

if (UNIX)
    message(Adding ALSA)
    find_package(ALSA REQUIRED)
    target_link_libraries(soloud ALSA::ALSA)
    target_compile_definitions(soloud PUBLIC -DWITH_ALSA)
elseif (WIN32)
    message(Adding Windows Multimedia)
    target_compile_definitions(soloud PUBLIC -DWITH_WINMM)
    target_link_libraries(soloud winMM.Lib)
endif ()
