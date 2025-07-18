#pragma once

#include <scene/Scene.h>
#include <scene/InputDeviceKey.h>

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/camera/FpsCamera.h>
#include <scene/renderer3d/camera/FreeCamera.h>
#include <scene/renderer3d/camera/CameraSpaceService.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/builder/ModelBuilder.h>
#include <scene/renderer3d/lighting/light/sun/SunLight.h>
#include <scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h>
#include <scene/renderer3d/lighting/light/spot/SpotLight.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingQuality.h>
#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <scene/renderer3d/landscape/fog/Fog.h>
#include <scene/renderer3d/landscape/terrain/Terrain.h>
#include <scene/renderer3d/landscape/terrain/TerrainMesh.h>
#include <scene/renderer3d/landscape/terrain/TerrainMaterial.h>
#include <scene/renderer3d/landscape/terrain/grass/TerrainGrass.h>
#include <scene/renderer3d/landscape/water/Water.h>

#include <scene/ui/EventListener.h>
#include <scene/ui/UIRenderer.h>
#include <scene/ui/Clipboard.h>
#include <scene/ui/widget/TextFieldConst.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/container/LazyWidget.h>
#include <scene/ui/widget/window/Window.h>
#include <scene/ui/widget/button/Button.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/textbox/TextBox.h>
#include <scene/ui/widget/textarea/Textarea.h>
#include <scene/ui/widget/sequence/Sequence.h>
#include <scene/ui/widget/slider/Slider.h>
#include <scene/ui/widget/checkbox/CheckBox.h>
#include <scene/ui/animation/UIAnimationSequencer.h>
#include <scene/ui/animation/UIAnimationParallelizer.h>
#include <scene/ui/animation/UIAnimationTranslate.h>
#include <scene/ui/animation/UIAnimationZoom.h>
#include <scene/ui/animation/UIAnimationRotation.h>
#include <scene/ui/animation/UIAnimationDelay.h>
#include <scene/ui/animation/UIAnimationFade.h>

#include <i18n/LabelTranslator.h>
#include <i18n/ParameterizedText.h>

#include <resources/ResourceRetriever.h>
#include <resources/material/MaterialBuilder.h>
#include <resources/geometry/GeometryModel.h>
#include <resources/geometry/line/LineModel.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <resources/geometry/capsule/CapsuleModel.h>
#include <resources/geometry/cylinder/CylinderModel.h>
#include <resources/geometry/cone/ConeModel.h>
#include <resources/geometry/obbox/OBBoxModel.h>
#include <resources/geometry/triangles/TrianglesModel.h>
#include <resources/geometry/sphere/SphereModel.h>
#include <resources/geometry/convexhull/ConvexHullModel.h>
#include <resources/geometry/frustum/FrustumModel.h>

#include <loader/image/LoaderPNG.h>

#include <graphics/render/shader/ShaderConfig.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/PipelineProcessorComparator.h>
#include <graphics/api/GraphicsApi.h>
