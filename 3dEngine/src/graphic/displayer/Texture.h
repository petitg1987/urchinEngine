#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

#include "graphic/displayer/TextureParam.h"

namespace urchin {

    class Texture {
        public:
            enum Type {
                DEFAULT,
                ARRAY,
                CUBE_MAP
            };

            static Texture build(unsigned int);
            static Texture build(unsigned int, Type);
            static Texture build(unsigned int, Type, TextureParam);

            unsigned int getId() const;
            unsigned int getGlType() const;
            TextureParam getParam() const;

        private:
            Texture(unsigned int, Type, TextureParam);

            unsigned int id;
            Type type;
            TextureParam param;
    };

}

#endif
