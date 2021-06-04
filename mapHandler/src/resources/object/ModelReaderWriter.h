#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

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
            static Model* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOn(const std::shared_ptr<XmlChunk>&, const Model*, XmlWriter&);

        private:
            static void loadAnimationsOn(Model*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeAnimationsOn(const std::shared_ptr<XmlChunk>&, const Model*, XmlWriter&);

            static void loadTransformOn(Model*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeTransformOn(const std::shared_ptr<XmlChunk>&, const Model*, XmlWriter&);

            static void loadFlagsOn(Model*, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeFlagsOn(const std::shared_ptr<XmlChunk>&, const Model*, XmlWriter&);
    };

}
