#ifndef URCHINENGINE_SKYBOX_H
#define URCHINENGINE_SKYBOX_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "graphic/render/GenericRenderer.h"
#include "resources/image/Image.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class Skybox {
        public:
            explicit Skybox(const std::vector<std::string>&);
            ~Skybox();

            void onCameraProjectionUpdate(const Matrix4<float>&) const;

            float getOffsetY() const;
            void setOffsetY(float);
            const std::vector<std::string>& getFilenames() const;

            void display(const RenderTarget*, const Matrix4<float>&, const Point3<float>&);

        private:
            void initialize();
            void clearSkyboxImages();

            std::vector<std::string> filenames;
            Image **skyboxImages;

            float offsetY;
            Matrix4<float> translationMatrix;

            //visual
            std::unique_ptr<GenericRenderer> skyboxRenderer;

            std::unique_ptr<Shader> skyboxShader;
            ShaderVar mProjectionShaderVar, mViewShaderVar;
    };

}

#endif
