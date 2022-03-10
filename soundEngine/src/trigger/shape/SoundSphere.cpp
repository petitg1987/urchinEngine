#include <trigger/shape/SoundSphere.h>

namespace urchin {

    /**
     * @param margin Margin to sphere used to determine stop sound trigger
     */
    SoundSphere::SoundSphere(float radius, const Point3<float>& position, float margin) :
            SoundShape(margin),
            playTriggerSphere(Sphere<float>(radius, position)),
            stopTriggerSphere(Sphere<float>(radius + margin, position)) {

    }

    SoundShape::ShapeType SoundSphere::getShapeType() const {
        return SoundShape::SPHERE_SHAPE;
    }

    const Sphere<float>& SoundSphere::getPlayTriggerSphere() const {
        return playTriggerSphere;
    }

    const Sphere<float>& SoundSphere::getStopTriggerSphere() const {
        return stopTriggerSphere;
    }

    float SoundSphere::getRadius() const {
        return playTriggerSphere.getRadius();
    }

    Point3<float> SoundSphere::getPosition() const {
        return playTriggerSphere.getCenterOfMass();
    }

    void SoundSphere::updatePosition(const Point3<float>& position) {
        playTriggerSphere = Sphere<float>(playTriggerSphere.getRadius(), position);
        stopTriggerSphere = Sphere<float>(stopTriggerSphere.getRadius(), position);
    }

    bool SoundSphere::pointInsidePlayShape(const Point3<float>& point) const {
        return playTriggerSphere.collideWithPoint(point);
    }

    bool SoundSphere::pointInsideStopShape(const Point3<float>& point) const {
        return stopTriggerSphere.collideWithPoint(point);
    }

    std::unique_ptr<SoundShape> SoundSphere::clone() const {
        return std::make_unique<SoundSphere>(getRadius(), getPosition(), getMargin());
    }

}
