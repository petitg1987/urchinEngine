#ifndef ENGINE_CONSTANIMATION_H
#define ENGINE_CONSTANIMATION_H

#include "UrchinCommon.h"

#include "resources/model/ConstMeshes.h"
#include "resources/Resource.h"

namespace urchin
{
	
	struct AnimationInformation
	{
		int currFrame;
		int nextFrame;
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
			ConstAnimation(const std::string &, unsigned int, unsigned int, unsigned int, const Bone *const *const, const AABBox<float> *const *const);
			~ConstAnimation();
			
			const std::string &getAnimationFilename() const;
			unsigned int getNumberFrames() const;
			unsigned int getNumberBones() const;
			unsigned int getFrameRate() const;
			const Bone &getBone(int, int) const;
			const AABBox<float> &getOriginalGlobalAABBox() const;

		private:
			std::string animationFilename;
			const unsigned int numFrames, numBones, frameRate;
			const Bone *const *const skeletonFrames;
			const AABBox<float> *const *const bboxes; //bounding boxes of each animation frames (not transformed)
			AABBox<float> *originalGlobalBBox; //original global bounding box (not transformed)
	};

}

#endif
