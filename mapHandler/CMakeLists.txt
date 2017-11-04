cmake_minimum_required(VERSION 3.7)
project(urchinMapHandler)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/ai/AIWorldGenerator.cpp
        src/ai/AIWorldGenerator.h
        src/resources/common/OrientationReaderWriter.cpp
        src/resources/common/OrientationReaderWriter.h
        src/resources/light/LightReaderWriter.cpp
        src/resources/light/LightReaderWriter.h
        src/resources/light/SceneLight.cpp
        src/resources/light/SceneLight.h
        src/resources/object/bodyshape/CollisionBoxReaderWriter.cpp
        src/resources/object/bodyshape/CollisionBoxReaderWriter.h
        src/resources/object/bodyshape/CollisionCapsuleReaderWriter.cpp
        src/resources/object/bodyshape/CollisionCapsuleReaderWriter.h
        src/resources/object/bodyshape/CollisionCompoundShapeReaderWriter.cpp
        src/resources/object/bodyshape/CollisionCompoundShapeReaderWriter.h
        src/resources/object/bodyshape/CollisionConeReaderWriter.cpp
        src/resources/object/bodyshape/CollisionConeReaderWriter.h
        src/resources/object/bodyshape/CollisionConvexHullReaderWriter.cpp
        src/resources/object/bodyshape/CollisionConvexHullReaderWriter.h
        src/resources/object/bodyshape/CollisionCylinderReaderWriter.cpp
        src/resources/object/bodyshape/CollisionCylinderReaderWriter.h
        src/resources/object/bodyshape/CollisionShapeReaderWriter.cpp
        src/resources/object/bodyshape/CollisionShapeReaderWriter.h
        src/resources/object/bodyshape/CollisionShapeReaderWriterRetriever.cpp
        src/resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h
        src/resources/object/bodyshape/CollisionSphereReaderWriter.cpp
        src/resources/object/bodyshape/CollisionSphereReaderWriter.h
        src/resources/object/ModelReaderWriter.cpp
        src/resources/object/ModelReaderWriter.h
        src/resources/object/RigidBodyReaderWriter.cpp
        src/resources/object/RigidBodyReaderWriter.h
        src/resources/object/SceneObject.cpp
        src/resources/object/SceneObject.h
        src/resources/sound/soundshape/SoundBoxReaderWriter.cpp
        src/resources/sound/soundshape/SoundBoxReaderWriter.h
        src/resources/sound/soundshape/SoundShapeReaderWriter.cpp
        src/resources/sound/soundshape/SoundShapeReaderWriter.h
        src/resources/sound/soundshape/SoundShapeReaderWriterRetriever.cpp
        src/resources/sound/soundshape/SoundShapeReaderWriterRetriever.h
        src/resources/sound/soundshape/SoundSphereReaderWriter.cpp
        src/resources/sound/soundshape/SoundSphereReaderWriter.h
        src/resources/sound/SceneSound.cpp
        src/resources/sound/SceneSound.h
        src/resources/sound/SoundReaderWriter.cpp
        src/resources/sound/SoundReaderWriter.h
        src/resources/sound/SoundTriggerReaderWriter.cpp
        src/resources/sound/SoundTriggerReaderWriter.h
        src/resources/Map.cpp
        src/resources/Map.h
        src/MapHandler.cpp
        src/MapHandler.h
        src/UrchinMapHandler.h src/resources/ai/SceneAI.cpp
        src/resources/ai/SceneAI.h
        src/resources/ai/NavMeshConfigWriter.cpp
        src/resources/ai/NavMeshConfigWriter.h
        src/resources/terrain/TerrainReaderWriter.cpp
        src/resources/terrain/TerrainReaderWriter.h
        src/resources/terrain/SceneTerrain.cpp
        src/resources/terrain/SceneTerrain.h)

include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../AIEngine/src)

add_library(urchinMapHandler SHARED ${SOURCE_FILES})
target_link_libraries(urchinMapHandler urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine -static-libstdc++)