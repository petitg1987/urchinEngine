#pragma once

#include <string>
#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <api/render/shader/model/Shader.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/GenericRenderer.h>
#include <resources/image/Image.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class Skybox {
        public:
            explicit Skybox(const std::vector<std::string>&);

            void initialize(RenderTarget&);

            const std::vector<std::string>& getFilenames() const;

            float getOffsetY() const;
            void setOffsetY(float);

            void prepareRendering(unsigned int&, const Matrix4<float>&, const Point3<float>&);

        private:
            bool isInitialized;
            std::vector<std::string> filenames;

            std::array<std::shared_ptr<Image>, 6> skyboxImages;

            float offsetY;
            Matrix4<float> translationMatrix;

            //visual
            std::unique_ptr<GenericRenderer> skyboxRenderer;
            std::unique_ptr<Shader> skyboxShader;
    };

}
