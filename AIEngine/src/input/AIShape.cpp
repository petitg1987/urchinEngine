#include <utility>

#include "AIShape.h"

namespace urchin {

    AIShape::AIShape(const ConvexShape3D<float>* shape) :
        shape(shape->clone()),
        bHasLocalTransform(false) {

    }

    AIShape::AIShape(const ConvexShape3D<float>* shape, Transform<float> localTransform) :
        shape(shape->clone()),
        bHasLocalTransform(true),
        localTransform(std::move(localTransform)) {

    }

    AIShape::~AIShape() {
        delete shape;
    }

    const ConvexShape3D<float>* AIShape::getShape() const {
        return shape;
    }

    bool AIShape::hasLocalTransform() const {
        return bHasLocalTransform;
    }

    const Transform<float>& AIShape::getLocalTransform() const {
        return localTransform;
    }

}
