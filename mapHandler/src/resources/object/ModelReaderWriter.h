#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class ModelReaderWriter {
        public:
            static Model* loadFrom(const XmlChunk*, const DataParser&);
            static void writeOn(XmlChunk*, const Model*, DataWriter&);

        private:
            static void loadAnimationsOn(Model*, const XmlChunk*, const DataParser&);
            static void writeAnimationsOn(const XmlChunk*, const Model*, DataWriter&);

            static void loadTransformOn(Model*, const XmlChunk*, const DataParser&);
            static void writeTransformOn(const XmlChunk*, const Model*, DataWriter&);

            static void loadFlagsOn(Model*, const XmlChunk*, const DataParser&);
            static void writeFlagsOn(const XmlChunk*, const Model*, DataWriter&);

            static constexpr char MESH_TAG[] = "mesh";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char ANIMATIONS_TAG[] = "animations";
            static constexpr char ANIMATION_TAG[] = "animation";
            static constexpr char NAME_TAG[] = "name";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char SCALE_TAG[] = "scale";
            static constexpr char PRODUCE_SHADOW_TAG[] = "produceShadow";
    };

}
