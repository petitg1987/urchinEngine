#include "AIShape.h"

namespace urchin
{

    AIShape::AIShape(std::shared_ptr<const ConvexShape3D<float>> shape) :
        shape(std::move(shape)),
        bHasLocalTransform(false)
    {

    }

    AIShape::AIShape(std::shared_ptr<const ConvexShape3D<float>> shape, Transform<float> localTransform) :
        shape(std::move(shape)),
        bHasLocalTransform(true),
        localTransform(std::move(localTransform))
    {

    }

    const std::shared_ptr<const ConvexShape3D<float>> &AIShape::getShape() const
    {
        return shape;
    }

    bool AIShape::hasLocalTransform() const
    {
        return bHasLocalTransform;
    }

    const Transform<float> &AIShape::getLocalTransform() const
    {
        return localTransform;
    }

}
