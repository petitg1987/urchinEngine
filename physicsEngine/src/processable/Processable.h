#pragma once

#include <UrchinCommon.h>

namespace urchin {

    /**
    * A processable will be processed in process loop of a physics world. Note that process execution is
    * done in the physics thread: make sure the concrete class is thread safe.
    */
    class Processable {
        public:
            virtual ~Processable() = default;

            virtual void setup(float, const Vector3<float>&) = 0;
            virtual void execute(float, const Vector3<float>&) = 0;
    };

}
