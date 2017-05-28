cmake_minimum_required(VERSION 3.7)
project(urchinSoundEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenAL)

set(SOURCE_FILES
        src/behavior/SmoothStopAction.cpp
        src/behavior/SmoothStopAction.h
        src/behavior/SoundBehavior.cpp
        src/behavior/SoundBehavior.h
        src/device/DeviceManager.cpp
        src/device/DeviceManager.h
        src/player/filereader/SoundFileReader.cpp
        src/player/filereader/SoundFileReader.h
        src/player/stream/AudioStreamPlayer.cpp
        src/player/stream/AudioStreamPlayer.h
        src/player/stream/StreamChunk.cpp
        src/player/stream/StreamChunk.h
        src/player/stream/StreamUpdateTask.cpp
        src/player/stream/StreamUpdateTask.h
        src/player/stream/StreamUpdateWorker.cpp
        src/player/stream/StreamUpdateWorker.h
        src/player/AudioPlayer.cpp
        src/player/AudioPlayer.h
        src/sound/ambient/AmbientSound.cpp
        src/sound/ambient/AmbientSound.h
        src/sound/spatial/PointSound.cpp
        src/sound/spatial/PointSound.h
        src/sound/Sound.cpp
        src/sound/Sound.h
        src/trigger/shape/SoundBox.cpp
        src/trigger/shape/SoundBox.h
        src/trigger/shape/SoundShape.cpp
        src/trigger/shape/SoundShape.h
        src/trigger/shape/SoundSphere.cpp
        src/trigger/shape/SoundSphere.h
        src/trigger/ManualTrigger.cpp
        src/trigger/ManualTrigger.h
        src/trigger/ShapeTrigger.cpp
        src/trigger/ShapeTrigger.h
        src/trigger/SoundTrigger.cpp
        src/trigger/SoundTrigger.h
        src/AudioController.cpp
        src/AudioController.h
        src/SoundManager.cpp
        src/SoundManager.h
        src/UrchinSoundEngine.h)

include_directories(src)
include_directories(../common/src)

add_library(urchinSoundEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinSoundEngine pthread openal sndfile urchinCommon -static-libstdc++)