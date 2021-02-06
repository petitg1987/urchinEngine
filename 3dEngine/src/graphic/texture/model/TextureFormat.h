#ifndef URCHINENGINE_TEXTUREFORMAT_H
#define URCHINENGINE_TEXTUREFORMAT_H

namespace urchin {

    enum TextureFormat {
        DEPTH_32_FLOAT,

        GRAYSCALE_8_INT,
        GRAYSCALE_16_FLOAT,

        RG_32_FLOAT,

        RGB_8_INT, //TODO remove it
        RGBA_8_INT
    };

}

#endif
