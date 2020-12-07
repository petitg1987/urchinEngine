#ifndef URCHINENGINE_TEXTUREPARAM_H
#define URCHINENGINE_TEXTUREPARAM_H

#include <GL/glew.h>

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

            static TextureParam buildNearest();
            static TextureParam buildLinear();
            static TextureParam buildRepeatNearest();
            static TextureParam buildRepeatLinear();
            static TextureParam build(ReadMode, ReadQuality, Anisotropy);

            GLint getGlReadMode() const;
            GLint getGlReadQuality(bool) const;

            bool needAnisotropy() const;

        private:
            TextureParam(ReadMode, ReadQuality, Anisotropy);

            ReadMode readMode;
            ReadQuality readQuality;
            Anisotropy anisotropy;
    };

}

#endif
