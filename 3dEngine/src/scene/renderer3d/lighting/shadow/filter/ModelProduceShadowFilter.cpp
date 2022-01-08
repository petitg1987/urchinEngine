#include <scene/renderer3d/lighting/shadow/filter/ModelProduceShadowFilter.h>

namespace urchin {

    bool ModelProduceShadowFilter::isAccepted(const Model *const model, const ConvexObject3D<float>&) const {
        //note: to filter by individual model (instead of all models belong to an octree): convexObject.collideWithAABBox(model->getAABBox())

        //receiver only are required for variance shadow map to work correctly (see 8.4.5: https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-8-summed-area-variance-shadow-maps)
        return model->getShadowClass() == Model::ShadowClass::RECEIVER_AND_CASTER || model->getShadowClass() == Model::ShadowClass::RECEIVER_ONLY;
    }

}
