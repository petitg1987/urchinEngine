#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundShapeReaderWriter {
        public:
            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char SPHERE_VALUE[] = "SPHERE";
            static constexpr char BOX_VALUE[] = "BOX";

            virtual ~SoundShapeReaderWriter() = default;

            virtual SoundShape* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const = 0;
            virtual void writeOn(const std::shared_ptr<XmlChunk>&, const SoundShape*, XmlWriter&) const = 0;
    };

}
