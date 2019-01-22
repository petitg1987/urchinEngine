#ifndef URCHINENGINE_NONPLAYERCHARACTER_H
#define URCHINENGINE_NONPLAYERCHARACTER_H

#include "UrchinCommon.h"

namespace urchin
{

    class NonPlayerCharacter
    {
        public:
            explicit NonPlayerCharacter(const Point3<float> &);

            void setVelocity(const Vector3<float> &);
            const Vector3<float> &getVelocity() const;

            void setPosition(const Point3<float> &);
            const Point3<float> &getPosition() const;

        private:
            Vector3<float> velocity;
            Point3<float> position;
    };

}

#endif
