#ifndef URCHINENGINE_MATHALGORITHM_H
#define URCHINENGINE_MATHALGORITHM_H

namespace urchin
{

	class MathAlgorithm
	{
		public:
			template<class T> static T clamp(T, T, T);
			template<class T> static T sign(T);

			static int nextPowerOfTwo(int);
			static unsigned int powerOfTwo(unsigned int);

			static bool isZero(float);

			template<class T> static T roundDivision(T, T);
	};

}

#endif
