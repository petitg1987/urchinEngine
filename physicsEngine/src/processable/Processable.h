#ifndef URCHINENGINE_PROCESSABLE_H
#define URCHINENGINE_PROCESSABLE_H

#include "UrchinCommon.h"

namespace urchin
{

    class PhysicsWorld;

    /**
    * A processable will be processed in process loop of a physics world. Note that process execution is
    * done in the physics thread: make sure the concrete class is thread safe.
    */
    class Processable
    {
        public:
            virtual ~Processable() = default;

            virtual void initialize(PhysicsWorld *) = 0;

            virtual void setup(float, const Vector3<float> &) = 0;
            virtual void execute(float, const Vector3<float> &) = 0;
    };

}

#endif
