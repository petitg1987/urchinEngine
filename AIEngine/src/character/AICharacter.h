#ifndef URCHINENGINE_AICHARACTER_H
#define URCHINENGINE_AICHARACTER_H

#include "UrchinCommon.h"

namespace urchin {

    class AICharacter {
        public:
            AICharacter(float, float, const Point3<float> &);

            float getMass() const;
            float getMaxVelocityInKmH() const;

            float retrieveMaxVelocityInMs() const;
            float retrieveMaxForce() const;
            float retrieveMaxMomentum() const;

            void updateMomentum(const Vector3<float> &);
            const Vector3<float> &getMomentum() const;

            void updatePosition(const Point3<float> &);
            const Point3<float> &getPosition() const;

        private:
            float mass;
            float maxVelocityInKmH;
            Vector3<float> momentum;
            Point3<float> position;
    };

}

#endif
