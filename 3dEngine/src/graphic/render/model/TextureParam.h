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

            static TextureParam buildNearest();
            static TextureParam buildLinear();
            static TextureParam buildRepeatNearest();
            static TextureParam buildRepeatLinear();

            unsigned int getGlReadMode() const;
            unsigned int getGlReadQuality() const;

        private:
            TextureParam(ReadMode, ReadQuality);

            ReadMode readMode;
            ReadQuality readQuality;
    };

}

#endif
