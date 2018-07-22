#ifndef URCHINENGINE_LOADERURCHINANIM_H
#define URCHINENGINE_LOADERURCHINANIM_H

#include <string>

#include "resources/model/ConstAnimation.h"
#include "loader/Loader.h"

namespace urchin
{

	struct BoneInfo
	{
		std::string name;
		int parent;
		unsigned int flags;
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
			~LoaderUrchinAnim() override = default;

			ConstAnimation *loadFromFile(const std::string &, void *) override;
	};

}

#endif
