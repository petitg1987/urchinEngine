#include "SplitBoundingBox.h"

namespace urchin
{

	SplitBoundingBox::SplitBoundingBox() :
		limitSize(ConfigService::instance()->getFloatValue("model.boxLimitSize"))
	{

	}

	SplitBoundingBox::~SplitBoundingBox()
	{

	}

	/**
	 * Split the bounding box in several bounding boxes at limitSize.
	 * If original box doesn't exceed limit size, the original box is returned.
	 */
	std::vector<AABBox<float>> SplitBoundingBox::split(const AABBox<float> &aabbox) const
	{
		std::vector<float> axisSplits[3];
		for(unsigned int axis=0; axis<3; ++axis)
		{
			float size = aabbox.getHalfSize(axis) * 2.0f;
			axisSplits[axis].push_back(aabbox.getMin()[axis]);

			int nbSplits = static_cast<int>(std::ceil(size/limitSize));
			float maxValue = aabbox.getMax()[axis];

			for(int split = 0; split < nbSplits; ++split)
			{
				float splitValue = std::min(aabbox.getMin()[axis] + limitSize*(split+1), maxValue);
				axisSplits[axis].push_back(splitValue);
			}
		}

		std::vector<AABBox<float>> splittedBoundingBox;
		for(unsigned int x=1; x<axisSplits[0].size(); ++x)
		{
			for(unsigned int y=1; y<axisSplits[1].size(); ++y)
			{
				for(unsigned int z=1; z<axisSplits[2].size(); ++z)
				{
					Point3<float> minPoint(axisSplits[0][x-1], axisSplits[1][y-1], axisSplits[2][z-1]);
					Point3<float> maxPoint(axisSplits[0][x], axisSplits[1][y], axisSplits[2][z]);

					splittedBoundingBox.push_back(AABBox<float>(minPoint, maxPoint));
				}
			}
		}

		return splittedBoundingBox;
	}

}
