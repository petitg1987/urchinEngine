#ifndef URCHINENGINE_MODELSETDISPLAYER_H
#define URCHINENGINE_MODELSETDISPLAYER_H

#include <vector>
#include <map>
#include <string>
#include "UrchinCommon.h"

#include "CustomModelShaderVariable.h"
#include "graphic/render/shader/model/Shader.h"
#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/DisplayMode.h"
#include "scene/renderer3d/camera/Camera.h"
#include "ModelDisplayer.h"

namespace urchin {

    /**
    * Allow to display models given in parameter depending of a configuration (view matrix, projection matrix, display mode...)
    */
    class ModelSetDisplayer {
        public:
            explicit ModelSetDisplayer(DisplayMode displayMode);
            ~ModelSetDisplayer();

            void initialize(std::shared_ptr<RenderTarget>);
            void onCameraProjectionUpdate(const Camera*);

            void setCustomShader(const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);
            void setCustomModelShaderVariable(CustomModelShaderVariable*);

            void setModels(const std::vector<Model*>&);
            void removeModel(Model*);

            void updateAnimation(float);
            void prepareRendering(const Matrix4<float>&);

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&) const;
            void drawBaseBones(const Matrix4<float>&, const Matrix4<float>&, const std::string&) const;

        private:
            bool isInitialized;

            std::string geometryShaderName, fragmentShaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            DisplayMode displayMode;
            std::shared_ptr<Shader> modelShader;
            Matrix4<float> projectionMatrix;

            CustomModelShaderVariable* customModelShaderVariable;

            std::shared_ptr<RenderTarget> renderTarget;
            std::vector<Model*> models;
            std::unordered_map<Model*, std::unique_ptr<ModelDisplayer>> modelsDisplayer;
    };

}

#endif
