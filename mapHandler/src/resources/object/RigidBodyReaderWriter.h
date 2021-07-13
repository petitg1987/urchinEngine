#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class RigidBodyReaderWriter {
        public:
            static RigidBody* loadFrom(const DataChunk*, const std::string&, const Transform<float>&, const DataParser&);
            static void writeOn(DataChunk&, const RigidBody*, DataWriter&);

        private:
            static void loadBodyPropertiesOn(RigidBody*, const DataChunk*, const DataParser&);
            static void writeBodyPropertiesOn(DataChunk&, const RigidBody*, DataWriter&);

            static constexpr char SHAPE_TAG[] = "shape";
            static constexpr char MASS_TAG[] = "mass";
            static constexpr char RESTITUTION_TAG[] = "restitution";
            static constexpr char FRICTION_TAG[] = "friction";
            static constexpr char ROLLING_FRICTION_TAG[] = "rollingFriction";
            static constexpr char LINEAR_DAMPING_TAG[] = "linearDamping";
            static constexpr char ANGULAR_DAMPING_TAG[] = "angularDamping";
            static constexpr char LINEAR_FACTOR_TAG[] = "linearFactor";
            static constexpr char ANGULAR_FACTOR_TAG[] = "angularFactor";
    };

}
