#include "ModelProduceShadowFilter.h"

namespace urchin
{

	ModelProduceShadowFilter::ModelProduceShadowFilter()
	{

	}

	ModelProduceShadowFilter::~ModelProduceShadowFilter()
	{

	}

	bool ModelProduceShadowFilter::isAccepted(const Model *const model) const
	{
		return model->isProduceShadow();
	}

}
