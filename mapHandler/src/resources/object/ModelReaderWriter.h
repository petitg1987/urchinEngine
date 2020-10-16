#ifndef URCHINENGINE_MODELREADERWRITER_H
#define URCHINENGINE_MODELREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin {

    class ModelReaderWriter {
        //XML tags
        #define MESH_TAG "mesh"
        #define FILENAME_TAG "filename"
        #define ANIMATIONS_TAG "animations"
        #define ANIMATION_TAG "animation"
        #define NAME_TAG "name"
        #define TRANSFORM_TAG "transform"
        #define POSITION_TAG "position"
        #define SCALE_TAG "scale"
        #define PRODUCE_SHADOW_TAG "produceShadow"

        public:
            Model *loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeOn(const std::shared_ptr<XmlChunk> &, const Model *, XmlWriter &) const;

        private:
            void loadAnimationsOn(Model *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeAnimationsOn(const std::shared_ptr<XmlChunk> &, const Model *, XmlWriter &) const;

            void loadTransformOn(Model *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeTransformOn(const std::shared_ptr<XmlChunk> &, const Model *, XmlWriter &) const;

            void loadFlagsOn(Model *, const std::shared_ptr<XmlChunk> &, const XmlParser &) const;
            void writeFlagsOn(const std::shared_ptr<XmlChunk> &, const Model *, XmlWriter &) const;
    };

}

#endif
