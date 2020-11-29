#ifndef URCHINENGINE_RIGIDBODYREADERWRITER_H
#define URCHINENGINE_RIGIDBODYREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

namespace urchin {

    class RigidBodyReaderWriter {
        //XML tags
        #define SHAPE_TAG "shape"
        #define MASS_TAG "mass"
        #define RESTITUTION_TAG "restitution"
        #define FRICTION_TAG "friction"
        #define ROLLING_FRICTION_TAG "rollingFriction"
        #define LINEAR_DAMPING_TAG "linearDamping"
        #define ANGULAR_DAMPING_TAG "angularDamping"
        #define LINEAR_FACTOR_TAG "linearFactor"
        #define ANGULAR_FACTOR_TAG "angularFactor"

        public:
            RigidBody *loadFrom(const std::shared_ptr<XmlChunk>&, const std::string&, const Transform<float>&, const XmlParser&) const;
            void writeOn(const std::shared_ptr<XmlChunk>&, const RigidBody*, XmlWriter&) const;

        private:
            void loadBodyPropertiesOn(RigidBody *, const std::shared_ptr<XmlChunk>&, const XmlParser&) const;
            void writeBodyPropertiesOn(const std::shared_ptr<XmlChunk>&, const RigidBody*, XmlWriter&) const;
    };

}

#endif
