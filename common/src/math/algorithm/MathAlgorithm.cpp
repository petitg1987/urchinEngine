#include <limits>
#include <cassert>
#include <algorithm>
#include <typeinfo>

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

	unsigned int MathAlgorithm::powerOfTwo(unsigned int exponent)
	{
		unsigned int result = 1;
		return result << exponent;
	}

	bool MathAlgorithm::isZero(float value)
	{
		return value > -std::numeric_limits<float>::epsilon() && value < std::numeric_limits<float>::epsilon();
	}

	/**
	 * Perform a division of two integers with a classical rounding.
	 */
	template<class T> T MathAlgorithm::roundDivision(T dividend, T divisor)
	{ //see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
		#ifdef _DEBUG
			assert(typeid(int)==typeid(T) || typeid(long)==typeid(T) || typeid(long long)==typeid(T));
    	#endif

		return (dividend + (divisor / 2)) / divisor;
	}

	//explicit template
	template float MathAlgorithm::clamp<float>(float, float, float);
	template float MathAlgorithm::sign<float>(float);
	template float MathAlgorithm::roundDivision<float>(float, float);

	template double MathAlgorithm::clamp<double>(double, double, double);
	template double MathAlgorithm::sign<double>(double);
	template double MathAlgorithm::roundDivision<double>(double, double);

	template int MathAlgorithm::clamp<int>(int, int, int);
	template int MathAlgorithm::sign<int>(int);
	template int MathAlgorithm::roundDivision<int>(int, int);

	template long long MathAlgorithm::clamp<long long>(long long, long long, long long);
	template long long MathAlgorithm::sign<long long>(long long);
	template long long MathAlgorithm::roundDivision<long long>(long long, long long);
}
