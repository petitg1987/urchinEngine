#pragma once

#include <model/ObjectEntity.h>

namespace urchin {

    class ObjectEntityReaderWriter {
        public:
            static std::unique_ptr<ObjectEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const ObjectEntity&, UdaWriter&);

        private:
            static constexpr char MODEL_TAG[] = "model";
            static constexpr char PHYSICS_TAG[] = "physics";
            static constexpr char TAGS_TAG[] = "tags";
            static constexpr char NAME_ATTR[] = "name";
    };

}
