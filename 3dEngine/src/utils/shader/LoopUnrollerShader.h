#ifndef URCHINENGINE_LOOPUNROLLERSHADER_H
#define URCHINENGINE_LOOPUNROLLERSHADER_H

#include <string>
#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Unroll loop in shader source.
	* A loop start with #LOOPx_START(y)# and end with #LOOPx_END# where 'x' is the number of the loop and 'y' the number of iteration.
	* Loop iterator number can be get with #LOOPx_COUNTER#. The next loop iteration can be stopped with #LOOPx_STOP#.
	* Conditional can be added on last iteration with #LOOPx_LAST_ITERATION_START# / #LOOPx_LAST_ITERATION_ELSE# / #LOOPx_LAST_ITERATION_END#.
	*/
	class LoopUnrollerShader
	{
		public:
			std::string unrollLoops(const std::string &);

		private:
			std::string unrollLoop(const std::string &, unsigned int, unsigned int);
	};

}

#endif
