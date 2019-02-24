#ifndef URCHINENGINE_NONPLAYERCHARACTER_H
#define URCHINENGINE_NONPLAYERCHARACTER_H

#include "UrchinCommon.h"

namespace urchin
{

    class NonPlayerCharacter
    {
        public:
            NonPlayerCharacter(float, float, const Point3<float> &);

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
