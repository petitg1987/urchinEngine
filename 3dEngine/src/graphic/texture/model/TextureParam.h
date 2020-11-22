#ifndef URCHINENGINE_TEXTUREPARAM_H
#define URCHINENGINE_TEXTUREPARAM_H

namespace urchin {

    class TextureParam {
        public:
            enum ReadMode {
                EDGE_CLAMP,
                REPEAT
            };

            enum ReadQuality {
                NEAREST,
                LINEAR,
                NEAREST_MIPMAP,
                LINEAR_MIPMAP
            };

            enum Anisotropy {
                NO_ANISOTROPY,
                ANISOTROPY
            };

            static TextureParam buildNearest();
            static TextureParam buildLinear();
            static TextureParam buildRepeatNearest();
            static TextureParam buildRepeatLinear();
            static TextureParam build(ReadMode, ReadQuality, Anisotropy);

            unsigned int getGlReadMode() const;

            unsigned int getGlReadQualityMinifyingFilter() const;
            unsigned int getGlReadQualityMagnificationFilter() const;

            bool needAnisotropy() const;

        private:
            TextureParam(ReadMode, ReadQuality, Anisotropy);

            ReadMode readMode;
            ReadQuality readQuality;
            Anisotropy anisotropy;
    };

}

#endif
