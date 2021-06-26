#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class AICharacter {
        public:
            AICharacter(float, float, const Point3<float>&);

            float getMass() const;
            float getMaxVelocityInKmH() const;

            float retrieveMaxVelocityInMs() const;
            float retrieveMaxForce() const;

            void updateVelocity(const Vector3<float>&);
            const Vector3<float>& getVelocity() const;

            void updatePosition(const Point3<float>&);
            const Point3<float>& getPosition() const;

        private:
            float mass;
            float maxVelocityInKmH;
            Vector3<float> velocity;
            Point3<float> position;
    };

}
