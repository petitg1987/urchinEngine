#include <limits>

#include <body/BodyMomentum.h>

namespace urchin {

    BodyMomentum::BodyMomentum(const Vector3<float>& momentum, const Vector3<float>& torqueMomentum) :
            momentum(momentum),
            torqueMomentum(torqueMomentum) {

    }

    const Vector3<float>& BodyMomentum::getMomentum() const {
        return momentum;
    }

    void BodyMomentum::addMomentum(const Vector3<float>& momentum) {
        this->momentum += momentum;
    }

    const Vector3<float>& BodyMomentum::getTorqueMomentum() const {
        return torqueMomentum;
    }

    void BodyMomentum::addTorqueMomentum(const Vector3<float>& torqueMomentum) {
        this->torqueMomentum += torqueMomentum;
    }

    bool BodyMomentum::hasMomentum() const {
        return momentum.squareLength() > std::numeric_limits<float>::epsilon() || torqueMomentum.squareLength() > std::numeric_limits<float>::epsilon();
    }

    void BodyMomentum::reset() {
        momentum.setNull();
        torqueMomentum.setNull();
    }

}
