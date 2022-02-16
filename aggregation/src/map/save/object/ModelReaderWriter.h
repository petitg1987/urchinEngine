#pragma once

#include <memory>
#include <string>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class ModelReaderWriter {
        public:
            static std::shared_ptr<Model> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const Model&, UdaWriter&);

        private:
            static void loadAnimations(Model&, const UdaChunk*, const UdaParser&);
            static void writeAnimations(UdaChunk&, const Model&, UdaWriter&);

            static void loadTransform(Model&, const UdaChunk*, const UdaParser&);
            static void writeTransform(UdaChunk&, const Model&, UdaWriter&);

            static void loadProperties(Model&, const UdaChunk*, const UdaParser&);
            static void writeProperties(UdaChunk&, const Model&, UdaWriter&);

            static constexpr char MESHES_TAG[] = "meshes";
            static constexpr char FILENAME_TAG[] = "filename";
            static constexpr char ANIMATIONS_TAG[] = "animations";
            static constexpr char ANIMATION_TAG[] = "animation";
            static constexpr char NAME_TAG[] = "name";
            static constexpr char ANIM_SHADOW_IMPACT[] = "shadowImpact";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char SCALE_TAG[] = "scale";
            static constexpr char SHADOW_CLASS_TAG[] = "shadowClass";
            static constexpr char RECEIVER_AND_CASTER_VALUE[] = "RECEIVER_AND_CASTER";
            static constexpr char RECEIVER_ONLY_VALUE[] = "RECEIVER_ONLY";
            static constexpr char NONE_VALUE[] = "NONE";
    };

}
