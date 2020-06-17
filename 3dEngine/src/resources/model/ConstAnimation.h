#ifndef URCHINENGINE_CONSTANIMATION_H
#define URCHINENGINE_CONSTANIMATION_H

#include "UrchinCommon.h"

#include "resources/Resource.h"
#include "resources/model/ConstMesh.h"

namespace urchin
{
	
	struct AnimationInformation
	{
		unsigned int currFrame;
		unsigned int nextFrame;
		float lastTime;
		float maxTime;
	};

	/**
	 * Contains all the constant/common data for an animation.
	 * Two identical models can use the instance of this class.
	 */
	class ConstAnimation : public Resource
	{
		public:
			ConstAnimation(std::string , unsigned int, unsigned int, unsigned int, const Bone *const *, const AABBox<float> *const *);
			~ConstAnimation() override;
			
			const std::string &getAnimationFilename() const;
			unsigned int getNumberFrames() const;
			unsigned int getNumberBones() const;
			unsigned int getFrameRate() const;
			const Bone &getBone(unsigned int, unsigned int) const;

			const AABBox<float> &getOriginalGlobalAABBox() const;
			const std::vector<AABBox<float>> &getOriginalGlobalSplitAABBoxes() const;

		private:
			std::string animationFilename;
			const unsigned int numFrames, numBones, frameRate;
			const Bone *const *const skeletonFrames;
			const AABBox<float> *const *const bboxes; //bounding boxes of each animation frames (not transformed)
			AABBox<float> originalGlobalBBox; //original global bounding box (not transformed)
			std::vector<AABBox<float>> originalGlobalSplitBBoxes;
	};

}

#endif
