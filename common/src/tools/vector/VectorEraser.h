#ifndef ENGINE_VECTORERASER_H
#define ENGINE_VECTORERASER_H

#include <vector>

namespace urchin
{

	class VectorEraser
	{
		public:
			template<class T> static void erase(std::vector<T> &, unsigned int);
			template<class T> static void erase(std::vector<T> &, typename std::vector<T>::iterator);

	};

	#include "VectorEraser.inl"

}

#endif
