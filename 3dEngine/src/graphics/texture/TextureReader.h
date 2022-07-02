#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class TextureReader {
        public:
            ~TextureReader();

            static std::shared_ptr<TextureReader> build(std::shared_ptr<Texture>, TextureParam);

            void initialize();

            const Texture* getTexture() const;
            const TextureParam& getParam() const;

        private:
            TextureReader(std::shared_ptr<Texture>, TextureParam);

            void cleanup();

            bool isInitialized;

            std::shared_ptr<Texture> texture;
            TextureParam param;
    };

}
