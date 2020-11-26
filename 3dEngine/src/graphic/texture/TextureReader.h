#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

#include "graphic/texture/Texture.h"
#include "graphic/texture/model/TextureType.h"
#include "graphic/texture/model/TextureParam.h"

namespace urchin {

    class TextureReader {
        public:
            static TextureReader build(std::shared_ptr<Texture>, TextureParam);

            unsigned int getId() const;
            TextureType getType() const;
            TextureParam getParam() const;

        private:
            TextureReader(std::shared_ptr<Texture>, TextureParam);

            std::shared_ptr<Texture> texture;
            TextureParam param;
    };

}

#endif
