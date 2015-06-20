#include "ProduceShadowFilter.h"

namespace urchin
{

	ProduceShadowFilter::ProduceShadowFilter()
	{

	}

	ProduceShadowFilter::~ProduceShadowFilter()
	{

	}

	bool ProduceShadowFilter::isAccepted(const Model *const model) const
	{
		return model->isProduceShadow();
	}

}
