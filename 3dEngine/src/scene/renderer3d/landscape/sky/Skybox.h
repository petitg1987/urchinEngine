#pragma once

#include <string>
#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "resources/image/Image.h"

namespace urchin {

    class Skybox {
        public:
            explicit Skybox(const std::vector<std::string>&);

            void initialize(RenderTarget&);

            const std::vector<std::string>& getFilenames() const;

            float getOffsetY() const;
            void setOffsetY(float);

            void prepareRendering(unsigned int, const Matrix4<float>&, const Point3<float>&);

        private:
            static constexpr uint32_t PROJ_VIEW_UNIFORM_BINDING = 0;
            static constexpr uint32_t SKYBOX_TEX_UNIFORM_BINDING = 1;

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
