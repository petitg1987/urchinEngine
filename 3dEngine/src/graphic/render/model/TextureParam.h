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
                LINEAR
            };

            enum Anisotropy {
                NO_ANISOTROPY,
                ANISOTROPY
            };

            enum MipMap {
                NO_MIPMAP,
                MIPMAP
            };

            static TextureParam buildNearest();
            static TextureParam buildLinear();
            static TextureParam buildRepeatNearest();
            static TextureParam buildRepeatLinear();
            static TextureParam build(ReadMode, ReadQuality, Anisotropy, MipMap);

            unsigned int getGlReadMode() const;

            unsigned int getGlReadQualityMinFilter() const;
            unsigned int getGlReadQuality() const;

            bool needAnisotropy() const;

            bool needMipMap() const;

        private:
            TextureParam(ReadMode, ReadQuality, Anisotropy, MipMap);

            ReadMode readMode;
            ReadQuality readQuality;
            Anisotropy anisotropy;
            MipMap mipMap;
    };

}

#endif
