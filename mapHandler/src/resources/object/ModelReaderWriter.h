#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class ModelReaderWriter {
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
