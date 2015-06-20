#ifndef ENGINE_IMPULSESOLVINGDATA_H
#define ENGINE_IMPULSESOLVINGDATA_H

#include "UrchinCommon.h"

namespace urchin
{

	struct ImpulseSolvingData
	{
		ImpulseSolvingData();
		~ImpulseSolvingData();

		float friction; //Friction factor. 0=no friction, 1=total friction

		float bias; //Bias to avoid bodies penetrate and for restitution

		float normalImpulseDenominator;
		float tangentImpulseDenominator;
	};

}

#endif
