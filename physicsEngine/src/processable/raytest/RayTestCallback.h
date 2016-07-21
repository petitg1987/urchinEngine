#ifndef ENGINE_RAYTESTCALLBACK_H
#define ENGINE_RAYTESTCALLBACK_H

#include <mutex>

namespace urchin
{

	class RayTestCallback
	{
		public:
			RayTestCallback();
			~RayTestCallback();

		private:
			mutable std::mutex mutex; //TODO use it

	};

}

#endif
