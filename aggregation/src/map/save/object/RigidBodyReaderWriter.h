#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class RigidBodyReaderWriter {
        public:
            static std::unique_ptr<RigidBody> load(const UdaChunk*, std::string, const Transform<float>&, const UdaParser&);
            static void write(UdaChunk&, const RigidBody&, UdaWriter&);

        private:
            static void loadBodyProperties(RigidBody&, const UdaChunk*, const UdaParser&);
            static void writeBodyProperties(UdaChunk&, const RigidBody&, UdaWriter&);

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
