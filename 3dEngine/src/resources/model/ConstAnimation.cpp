#include <limits>

#include "resources/model/ConstAnimation.h"
#include "resources/model/boundingbox/SplitBoundingBox.h"

namespace urchin
{
	
	ConstAnimation::ConstAnimation(const std::string &animationFilename, unsigned int numFrames, unsigned int numBones,
			unsigned int frameRate,	const Bone *const *const skeletonFrames, const AABBox<float>*const *const bboxes) :
		animationFilename(animationFilename),
		numFrames(numFrames),
		numBones(numBones),
		frameRate(frameRate),
		skeletonFrames(skeletonFrames),
		bboxes(bboxes)
	{
		//determines the bounding box
		Point3<float> globalMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point3<float> globalMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

		for(unsigned int i=0; i<numFrames; ++i)
		{
			if(globalMin.X > bboxes[i]->getMin().X)
			{
				globalMin.X = bboxes[i]->getMin().X;
			}
			if(globalMin.Y > bboxes[i]->getMin().Y)
			{
				globalMin.Y = bboxes[i]->getMin().Y;
			}
			if(globalMin.Z > bboxes[i]->getMin().Z)
			{
				globalMin.Z = bboxes[i]->getMin().Z;
			}

			if(globalMax.X < bboxes[i]->getMax().X)
			{
				globalMax.X = bboxes[i]->getMax().X;
			}
			if(globalMax.Y < bboxes[i]->getMax().Y)
			{
				globalMax.Y = bboxes[i]->getMax().Y;
			}
			if(globalMax.Z < bboxes[i]->getMax().Z)
			{
				globalMax.Z = bboxes[i]->getMax().Z;
			}
		}
		originalGlobalBBox = new AABBox<float>(globalMin, globalMax);
		originalGlobalSplittedBBox = SplitBoundingBox().split(*originalGlobalBBox);
	}

	ConstAnimation::~ConstAnimation()
	{
		for(unsigned int i=0;i<numFrames;i++)
		{
			delete [] skeletonFrames[i];
			delete bboxes[i];
		}

		delete [] skeletonFrames;
		delete [] bboxes;
		delete originalGlobalBBox;
	}

	const std::string &ConstAnimation::getAnimationFilename() const
	{
		return animationFilename;
	}

	unsigned int ConstAnimation::getNumberFrames() const
	{
		return numFrames;
	}

	unsigned int ConstAnimation::getNumberBones() const
	{
		return numBones;
	}

	unsigned int ConstAnimation::getFrameRate() const
	{
		return frameRate;
	}

	const Bone &ConstAnimation::getBone(int frameNumber, int boneNumber) const
	{
		return skeletonFrames[frameNumber][boneNumber];
	}

	const AABBox<float> &ConstAnimation::getOriginalGlobalAABBox() const
	{
		return *originalGlobalBBox;
	}

	const std::vector<AABBox<float>> &ConstAnimation::getOriginalGlobalSplittedAABBox() const
	{
		return originalGlobalSplittedBBox;
	}

}
