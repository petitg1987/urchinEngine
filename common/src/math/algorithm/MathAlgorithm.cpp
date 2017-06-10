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
	 * @return -1 if value is negative, 0 if value is 0, otherwise 1
	 */
	template<class T> T MathAlgorithm::sign(T value)
	{
		return ((T)0 < value) - (value < (T)0);
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

	unsigned int MathAlgorithm::powerOfTwo(unsigned int exponent)
	{
		unsigned int result = 1;
		return result << exponent;
	}

	//explicit template
	template float MathAlgorithm::clamp<float>(float, float, float);
	template float MathAlgorithm::sign<float>(float);

	template double MathAlgorithm::clamp<double>(double, double, double);
	template double MathAlgorithm::sign<double>(double);

	template int MathAlgorithm::clamp<int>(int, int, int);
	template int MathAlgorithm::sign<int>(int);

	template long long MathAlgorithm::clamp<long long>(long long, long long, long long);
	template long long MathAlgorithm::sign<long long>(long long);
}
