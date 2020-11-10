#ifndef URCHINENGINE_MODELDISPLAYER_H
#define URCHINENGINE_MODELDISPLAYER_H

#include <vector>
#include <map>
#include <string>
#include "UrchinCommon.h"

#include "MeshParameter.h"
#include "CustomUniform.h"
#include "CustomModelUniform.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    /**
    * Allow to display models given in parameter depending of a configuration (view matrix, projection matrix, display mode...)
    */
    class ModelDisplayer {
        public:
            enum DisplayMode {
                DEFAULT_MODE = 0,
                DEPTH_ONLY_MODE
            };

            explicit ModelDisplayer(DisplayMode displayMode);

            void initialize();
            void onCameraProjectionUpdate(const Camera *);

            ShaderVar getShaderVar(const std::string &name) const;
            void setCustomGeometryShader(const std::string &, const std::map<std::string, std::string> &);
            void setCustomFragmentShader(const std::string &, const std::map<std::string, std::string> &);
            void setCustomUniform(CustomUniform *);
            void setCustomModelUniform(CustomModelUniform *);
            void setModels(const std::vector<Model *> &);

            void updateAnimation(float);
            void display(const Matrix4<float> &);

            void drawBBox(const Matrix4<float> &, const Matrix4<float> &) const;
            void drawBaseBones(const Matrix4<float> &, const Matrix4<float> &, const std::string &) const;

        private:
            void createShader(const std::string &, const std::string &, const std::string &);

            bool isInitialized;

            std::string geometryShaderName, fragmentShaderName;
            std::map<std::string, std::string> geometryTokens, fragmentTokens;

            DisplayMode displayMode;
            MeshParameter meshParameter;
            std::unique_ptr<Shader> modelShader;
            Matrix4<float> projectionMatrix;
            ShaderVar mProjectionShaderVar, mModelShaderVar, mViewShaderVar, mNormalShaderVar, ambientFactorShaderVar;

            CustomUniform *customUniform;
            CustomModelUniform *customModelUniform;

            std::vector<Model *> models;
    };

}

#endif
