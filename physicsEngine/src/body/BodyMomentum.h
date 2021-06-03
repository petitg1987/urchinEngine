#ifndef URCHINENGINE_BODYMOMENTUM_H
#define URCHINENGINE_BODYMOMENTUM_H

#include <UrchinCommon.h>

namespace urchin {

    class BodyMomentum {
        public:
            BodyMomentum() = default;
            BodyMomentum(const Vector3<float>&, const Vector3<float>&);
            BodyMomentum(const BodyMomentum&) = default;

            const Vector3<float>& getMomentum() const;
            void addMomentum(const Vector3<float>&);

            const Vector3<float>& getTorqueMomentum() const;
            void addTorqueMomentum(const Vector3<float>&);

            bool hasMomentum() const;
            void reset();
        private:
            Vector3<float> momentum;
            Vector3<float> torqueMomentum;
    };

}

#endif
