#ifndef URCHINENGINE_MODELSETDISPLAYER_H
#define URCHINENGINE_MODELSETDISPLAYER_H

#include <vector>
#include <map>
#include <string>
#include "UrchinCommon.h"

#include "MeshParameter.h"
#include "CustomShaderVariable.h"
#include "CustomModelShaderVariable.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    /**
    * Allow to display models given in parameter depending of a configuration (view matrix, projection matrix, display mode...)
    */
    class ModelSetDisplayer {
        public:
            enum DisplayMode {
                DEFAULT_MODE = 0,
                DEPTH_ONLY_MODE
            };

            explicit ModelSetDisplayer(DisplayMode displayMode);

            void initialize();
            void onCameraProjectionUpdate(const Camera*);

            ShaderVar getShaderVar(const std::string&) const;
            void setCustomGeometryShader(const std::string&, const std::map<std::string, std::string>&);
            void setCustomFragmentShader(const std::string&, const std::map<std::string, std::string>&);
            void setCustomShaderVariable(CustomShaderVariable*);
            void setCustomModelShaderVariable(CustomModelShaderVariable*);

            void setRenderTarget(std::shared_ptr<RenderTarget>);
            void setModels(const std::vector<Model*>&);

            void updateAnimation(float);
            void display(const Matrix4<float>&);

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&) const;
            void drawBaseBones(const Matrix4<float>&, const Matrix4<float>&, const std::string&) const;

        private:
            void createShader(const std::string&, const std::string&, const std::string&);

            bool isInitialized;

            std::string geometryShaderName, fragmentShaderName;
            std::map<std::string, std::string> geometryTokens, fragmentTokens;

            DisplayMode displayMode;
            MeshParameter meshParameter;
            std::unique_ptr<Shader> modelShader;
            Matrix4<float> projectionMatrix;
            ShaderVar mProjectionShaderVar, mModelShaderVar, mViewShaderVar, mNormalShaderVar, ambientFactorShaderVar;

            CustomShaderVariable* customShaderVariable;
            CustomModelShaderVariable* customModelShaderVariable;

            std::shared_ptr<RenderTarget> renderTarget;
            std::vector<Model*> models;
    };

}

#endif
