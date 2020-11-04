#ifndef URCHINENGINE_URCHIN3DENGINE_H
#define URCHINENGINE_URCHIN3DENGINE_H

#include "scene/SceneManager.h"
#include "scene/InputDeviceKey.h"

#include "scene/renderer3d/light/LightManager.h"
#include "scene/renderer3d/shadow/ShadowManager.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/camera/FpsCamera.h"
#include "scene/renderer3d/camera/FreeCamera.h"
#include "scene/renderer3d/camera/CameraSpaceService.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/light/sun/SunLight.h"
#include "scene/renderer3d/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/antialiasing/AntiAliasingManager.h"
#include "scene/renderer3d/fog/Fog.h"
#include "scene/renderer3d/terrain/Terrain.h"
#include "scene/renderer3d/terrain/TerrainMesh.h"
#include "scene/renderer3d/terrain/TerrainMaterial.h"
#include "scene/renderer3d/terrain/grass/TerrainGrass.h"
#include "scene/renderer3d/water/Water.h"

#include "scene/GUI/EventListener.h"
#include "scene/GUI/GUIRenderer.h"
#include "scene/GUI/widget/container/Container.h"
#include "scene/GUI/widget/window/Window.h"
#include "scene/GUI/widget/button/Button.h"
#include "scene/GUI/widget/staticbitmap/StaticBitmap.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/widget/textbox/TextBox.h"
#include "scene/GUI/widget/slider/Slider.h"

#include "graphic/texture/TextureManager.h"

#include "graphic/shader/ShaderManager.h"

#include "graphic/displayer/geometry/GeometryModel.h"
#include "graphic/displayer/geometry/aabbox/AABBoxModel.h"
#include "graphic/displayer/geometry/capsule/CapsuleModel.h"
#include "graphic/displayer/geometry/cylinder/CylinderModel.h"
#include "graphic/displayer/geometry/cone/ConeModel.h"
#include "graphic/displayer/geometry/frustum/FrustumModel.h"
#include "graphic/displayer/geometry/obbox/OBBoxModel.h"
#include "graphic/displayer/geometry/points/PointsModel.h"
#include "graphic/displayer/geometry/lines/LinesModel.h"
#include "graphic/displayer/geometry/quads/QuadsModel.h"
#include "graphic/displayer/geometry/triangles/TrianglesModel.h"
#include "graphic/displayer/geometry/sphere/SphereModel.h"
#include "graphic/displayer/geometry/convexhull/ConvexHullModel.h"

#endif
