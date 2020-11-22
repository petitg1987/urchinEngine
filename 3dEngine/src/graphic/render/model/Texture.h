#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

#include "graphic/texture/model/TextureType.h"
#include "graphic/render/model/TextureParam.h"

namespace urchin {

    class Texture {
        public:
            static Texture build(unsigned int);
            static Texture build(unsigned int, TextureType);
            static Texture build(unsigned int, TextureType, TextureParam);

            unsigned int getId() const;
            TextureType getType() const;
            TextureParam getParam() const;

        private:
            Texture(unsigned int, TextureType, TextureParam);

            unsigned int id;
            TextureType type;
            TextureParam param;
    };

}

#endif
