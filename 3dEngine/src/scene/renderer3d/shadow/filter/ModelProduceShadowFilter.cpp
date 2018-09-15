#include "ModelProduceShadowFilter.h"

namespace urchin
{

	bool ModelProduceShadowFilter::isAccepted(const Model *const model, const ConvexObject3D<float> &convexObject) const
	{
		return model->isProduceShadow() && convexObject.collideWithAABBox(model->getAABBox());
	}

}
