#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingQuality.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/postprocess/antialiasing/FxaaApplier.h>
#include <scene/renderer3d/postprocess/antialiasing/TaaApplier.h>

namespace urchin {

    class AntiAliasingApplier {
        public:
            struct Config {
                bool useTaa = true;
                AntiAliasingQuality quality = AntiAliasingQuality::HIGH;
            };

            AntiAliasingApplier(const Config&, bool);

            void updateCamera(Camera&);

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyAntiAliasing(uint32_t, unsigned int) const;

        private:
            //properties
            bool isTestMode;
            Config config;

            //applier
            FxaaApplier fxaaApplier;
            TaaApplier taaApplier;
    };

}
