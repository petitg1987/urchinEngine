#ifndef ENGINE_URCHIN3DENGINE_H
#define ENGINE_URCHIN3DENGINE_H

#include "scene/SceneManager.h"

#include "scene/renderer3d/light/LightManager.h"
#include "scene/renderer3d/shadow/ShadowManager.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/camera/FpsCamera.h"
#include "scene/renderer3d/camera/FreeCamera.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/light/sun/SunLight.h"
#include "scene/renderer3d/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/antialiasing/AntiAliasingManager.h"

#include "scene/GUI/EventListener.h"
#include "scene/GUI/GUIRenderer.h"
#include "scene/GUI/window/Window.h"
#include "scene/GUI/button/Button.h"
#include "scene/GUI/staticbitmap/StaticBitmap.h"
#include "scene/GUI/text/Text.h"
#include "scene/GUI/textbox/TextBox.h"

#include "texture/TextureManager.h"

#include "utils/shader/ShaderManager.h"

#include "utils/display/geometry/GeometryModel.h"
#include "utils/display/geometry/aabbox/AABBoxModel.h"
#include "utils/display/geometry/capsule/CapsuleModel.h"
#include "utils/display/geometry/cylinder/CylinderModel.h"
#include "utils/display/geometry/frustum/FrustumModel.h"
#include "utils/display/geometry/obbox/OBBoxModel.h"
#include "utils/display/geometry/points/PointsModel.h"
#include "utils/display/geometry/sphere/SphereModel.h"

#endif
