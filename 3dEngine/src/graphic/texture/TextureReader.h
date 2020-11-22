#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

#include "graphic/texture/model/TextureType.h"
#include "graphic/texture/model/TextureParam.h"

namespace urchin {

    class TextureReader {
        public:
            static TextureReader build(unsigned int);
            static TextureReader build(unsigned int, TextureType);
            static TextureReader build(unsigned int, TextureType, TextureParam);

            unsigned int getId() const;
            TextureType getType() const;
            TextureParam getParam() const;

        private:
            TextureReader(unsigned int, TextureType, TextureParam);

            unsigned int id;
            TextureType type;
            TextureParam param;
    };

}

#endif
