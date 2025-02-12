#pragma once

#include <model/ObjectEntity.h>

namespace urchin {

    class ObjectEntityReaderWriter {
        public:
            static std::unique_ptr<ObjectEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const ObjectEntity&, UdaParser&);

        private:
            static constexpr char MODEL_TAG[] = "model";
            static constexpr char PHYSICS_TAG[] = "physics";
            static constexpr char LIGHT_TAG[] = "light";
            static constexpr char SOUND_COMPONENT_TAG[] = "soundComponent";
            static constexpr char TAGS_TAG[] = "tags";
            static constexpr char NAME_ATTR[] = "name";
    };

}
