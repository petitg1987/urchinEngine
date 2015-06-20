#ifndef ENGINE_ACCUMULATEDSOLVINGDATA_H
#define ENGINE_ACCUMULATEDSOLVINGDATA_H

#include "UrchinCommon.h"

namespace urchin
{

	struct AccumulatedSolvingData
	{
		AccumulatedSolvingData();
		~AccumulatedSolvingData();

		float accNormalImpulse;
		float accTangentImpulse;
	};

}

#endif
