#ifndef URCHINENGINE_TEXTUREREADER_H
#define URCHINENGINE_TEXTUREREADER_H

#include <memory>

#include <graphic/texture/Texture.h>
#include <graphic/texture/model/TextureParam.h>

namespace urchin {

    class TextureReader {
        public:
            ~TextureReader();

            static std::shared_ptr<TextureReader> build(std::shared_ptr<Texture>, const TextureParam&);

            void initialize();

            const std::shared_ptr<Texture>& getTexture() const;
            const TextureParam& getParam() const;

        private:
            TextureReader(std::shared_ptr<Texture>, const TextureParam&);

            void cleanup();

            bool isInitialized;

            std::shared_ptr<Texture> texture;
            TextureParam param;
    };

}

#endif
