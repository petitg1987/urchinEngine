#ifndef ENGINE_LOADERURCHINANIM_H
#define ENGINE_LOADERURCHINANIM_H

#include <string>
#include <fstream>

#include "resources/model/ConstAnimation.h"
#include "loader/Loader.h"

namespace urchin
{

	struct BoneInfo
	{
		std::string name;
		int parent;
		int flags;
		int startIndex;
	};

	struct BaseFrameBone
	{
		Point3<float> pos;
		Quaternion<float> orient;
	};

	class LoaderUrchinAnim : public Loader<ConstAnimation>
	{
		public:
			virtual ~LoaderUrchinAnim();

			ConstAnimation *loadFromFile(const std::string &, void *);
			
		private:
			void nextLine(std::ifstream &, std::string &);
	};

}

#endif
