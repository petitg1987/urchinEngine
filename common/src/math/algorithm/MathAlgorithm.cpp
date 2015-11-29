#include <limits>
#include <cassert>
#include <algorithm>

#include "math/algorithm/MathAlgorithm.h"

namespace urchin
{

	template<class T> T MathAlgorithm::clamp(T value, T minValue, T maxValue)
	{
		#ifdef _DEBUG
			assert((maxValue + std::numeric_limits<T>::epsilon()) > minValue);
		#endif

		return std::max(minValue, std::min(maxValue, value));
	}

	/**
	 * @return -1 if value is negative otherwise 1
	 */
	template<class T> T MathAlgorithm::sign(T value)
	{
		if(value < (T)0)
		{
			return (T)-1;
		}

		return (T)1;
	}

	int MathAlgorithm::nextPowerOfTwo(int val)
	{
		int valPow2=1;
		while(valPow2 < val)
		{
			valPow2<<=1;
		}
		return valPow2;
	}

	bool MathAlgorithm::isZero(float value)
	{
		return value > -std::numeric_limits<float>::epsilon() && value < std::numeric_limits<float>::epsilon();
	}

	unsigned int MathAlgorithm::pow2(unsigned int exponent)
	{
		unsigned int result = 1;
		for(unsigned int i=0; i<exponent; ++i)
		{
			result *= 2;
		}

		return result;
	}

	//explicit template
	template float MathAlgorithm::clamp<float>(float, float, float);
	template float MathAlgorithm::sign<float>(float);
}
