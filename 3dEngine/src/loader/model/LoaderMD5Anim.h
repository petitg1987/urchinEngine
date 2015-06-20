#ifndef ENGINE_LOADERMD5ANIM_H
#define ENGINE_LOADERMD5ANIM_H

#include <string>
#include <fstream>
#include "UrchinCommon.h"

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

	class LoaderMD5Anim : public Loader<ConstAnimation>
	{
		public:
			virtual ~LoaderMD5Anim();

			ConstAnimation *loadFromFile(const std::string &, void *);
			
		private:
			void nextLine(std::ifstream &, std::string &);
	};

}

#endif
