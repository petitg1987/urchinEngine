#include "ModelProduceShadowFilter.h"

namespace urchin
{

	bool ModelProduceShadowFilter::isAccepted(const Model *const model, const ConvexObject3D<float> &) const
	{
	    //Info: to filter by individual model (instead of all models belong to an octree): convexObject.collideWithAABBox(model->getAABBox())
		return model->isProduceShadow();
	}

}
