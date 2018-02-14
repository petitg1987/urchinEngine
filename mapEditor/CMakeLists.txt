cmake_minimum_required(VERSION 3.7)
project(urchinMapEditor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(OpenGL)

set(SOURCE_FILES
        src/dialog/NewDialog.cpp
        src/dialog/NewDialog.h
        src/dialog/NotSavedDialog.cpp
        src/dialog/NotSavedDialog.h
        src/scene/controller/ai/AIController.cpp
        src/scene/controller/ai/AIController.h
        src/scene/controller/ai/AIControllerWidget.cpp
        src/scene/controller/ai/AIControllerWidget.h
        src/scene/controller/lights/dialog/NewLightDialog.cpp
        src/scene/controller/lights/dialog/NewLightDialog.h
        src/scene/controller/lights/LightController.cpp
        src/scene/controller/lights/LightController.h
        src/scene/controller/lights/LightControllerWidget.cpp
        src/scene/controller/lights/LightControllerWidget.h
        src/scene/controller/lights/LightTableView.cpp
        src/scene/controller/lights/LightTableView.h
        src/scene/controller/objects/bodyshape/support/DefaultBodyShapeCreator.cpp
        src/scene/controller/objects/bodyshape/support/DefaultBodyShapeCreator.h
        src/scene/controller/objects/bodyshape/support/LocalizedShapeTableView.cpp
        src/scene/controller/objects/bodyshape/support/LocalizedShapeTableView.h
        src/scene/controller/objects/bodyshape/support/SpinBoxDelegate.cpp
        src/scene/controller/objects/bodyshape/support/SpinBoxDelegate.h
        src/scene/controller/objects/bodyshape/BodyBoxShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyBoxShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyCapsuleShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyCapsuleShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyCompoundShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyCompoundShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyConeShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyConeShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyConvexHullShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyConvexHullShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyCylinderShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyCylinderShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodyShapeWidget.h
        src/scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.cpp
        src/scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.h
        src/scene/controller/objects/bodyshape/BodySphereShapeWidget.cpp
        src/scene/controller/objects/bodyshape/BodySphereShapeWidget.h
        src/scene/controller/objects/bodyshape/NoBodyShapeWidget.cpp
        src/scene/controller/objects/bodyshape/NoBodyShapeWidget.h
        src/scene/controller/objects/dialog/ChangeBodyShapeDialog.cpp
        src/scene/controller/objects/dialog/ChangeBodyShapeDialog.h
        src/scene/controller/objects/dialog/NewObjectDialog.cpp
        src/scene/controller/objects/dialog/NewObjectDialog.h
        src/scene/controller/objects/ObjectController.cpp
        src/scene/controller/objects/ObjectController.h
        src/scene/controller/objects/ObjectControllerWidget.cpp
        src/scene/controller/objects/ObjectControllerWidget.h
        src/scene/controller/objects/ObjectTableView.cpp
        src/scene/controller/objects/ObjectTableView.h
        src/scene/controller/sounds/dialog/ChangeSoundShapeDialog.cpp
        src/scene/controller/sounds/dialog/ChangeSoundShapeDialog.h
        src/scene/controller/sounds/dialog/ChangeSoundTriggerDialog.cpp
        src/scene/controller/sounds/dialog/ChangeSoundTriggerDialog.h
        src/scene/controller/sounds/dialog/NewSoundDialog.cpp
        src/scene/controller/sounds/dialog/NewSoundDialog.h
        src/scene/controller/sounds/soundshape/support/DefaultSoundShapeCreator.cpp
        src/scene/controller/sounds/soundshape/support/DefaultSoundShapeCreator.h
        src/scene/controller/sounds/soundshape/SoundBoxShapeWidget.cpp
        src/scene/controller/sounds/soundshape/SoundBoxShapeWidget.h
        src/scene/controller/sounds/soundshape/SoundShapeWidget.cpp
        src/scene/controller/sounds/soundshape/SoundShapeWidget.h
        src/scene/controller/sounds/soundshape/SoundShapeWidgetRetriever.cpp
        src/scene/controller/sounds/soundshape/SoundShapeWidgetRetriever.h
        src/scene/controller/sounds/soundshape/SoundSphereShapeWidget.cpp
        src/scene/controller/sounds/soundshape/SoundSphereShapeWidget.h
        src/scene/controller/sounds/SoundController.cpp
        src/scene/controller/sounds/SoundController.h
        src/scene/controller/sounds/SoundControllerWidget.cpp
        src/scene/controller/sounds/SoundControllerWidget.h
        src/scene/controller/sounds/SoundTableView.cpp
        src/scene/controller/sounds/SoundTableView.h
        src/scene/controller/SceneController.cpp
        src/scene/controller/SceneController.h
        src/scene/controller/SceneControllerWidget.cpp
        src/scene/controller/SceneControllerWidget.h
        src/scene/displayer/ai/NavMeshDisplayer.cpp
        src/scene/displayer/ai/NavMeshDisplayer.h
        src/scene/displayer/lights/LightScopeDisplayer.cpp
        src/scene/displayer/lights/LightScopeDisplayer.h
        src/scene/displayer/objects/BodyShapeDisplayer.cpp
        src/scene/displayer/objects/BodyShapeDisplayer.h
        src/scene/displayer/sounds/SoundTriggerDisplayer.cpp
        src/scene/displayer/sounds/SoundTriggerDisplayer.h
        src/scene/displayer/SceneDisplayer.cpp
        src/scene/displayer/SceneDisplayer.h
        src/scene/displayer/SceneDisplayerWidget.cpp
        src/scene/displayer/SceneDisplayerWidget.h
        src/scene/displayer/SceneFreeCamera.cpp
        src/scene/displayer/SceneFreeCamera.h
        src/support/ButtonStyleHelper.cpp
        src/support/ButtonStyleHelper.h
        src/support/ComboBoxStyleHelper.cpp
        src/support/ComboBoxStyleHelper.h
        src/support/FrameStyleHelper.cpp
        src/support/FrameStyleHelper.h
        src/support/GroupBoxStyleHelper.cpp
        src/support/GroupBoxStyleHelper.h
        src/support/LabelStyleHelper.cpp
        src/support/LabelStyleHelper.h
        src/support/SpinBoxStyleHelper.cpp
        src/support/SpinBoxStyleHelper.h
        src/Main.cpp
        src/Main.h
        src/MapEditorWindow.cpp
        src/MapEditorWindow.h
        src/scene/controller/terrains/TerrainController.cpp
        src/scene/controller/terrains/TerrainController.h
        src/scene/controller/terrains/TerrainControllerWidget.cpp
        src/scene/controller/terrains/TerrainControllerWidget.h
        src/scene/controller/terrains/TerrainTableView.cpp
        src/scene/controller/terrains/TerrainTableView.h
        src/scene/controller/terrains/dialog/NewTerrainDialog.cpp
        src/scene/controller/terrains/dialog/NewTerrainDialog.h
        src/scene/controller/waters/WaterController.cpp
        src/scene/controller/waters/WaterController.h
        src/scene/controller/waters/WaterTableView.cpp
        src/scene/controller/waters/WaterTableView.h
        src/scene/controller/waters/WaterControllerWidget.cpp
        src/scene/controller/waters/WaterControllerWidget.h
        src/scene/controller/waters/dialog/NewWaterDialog.cpp
        src/scene/controller/waters/dialog/NewWaterDialog.h
        src/scene/controller/objects/dialog/CloneObjectDialog.cpp
        src/scene/controller/objects/dialog/CloneObjectDialog.h)

include_directories(src ../common/src ../3dEngine/src ../physicsEngine/src ../soundEngine/src ../AIEngine/src ../mapHandler/src)

add_custom_target(copyShaders rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../3dEngine/resources/shaders/* ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/)

add_executable(urchinMapEditor ${SOURCE_FILES})
add_dependencies(urchinMapEditor copyShaders)
qt5_use_modules(urchinMapEditor Widgets OpenGL)
target_link_libraries(urchinMapEditor ${QT_LIBRARIES} ${OPENGL_LIBRARIES} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine urchinMapHandler)