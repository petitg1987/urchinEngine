#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H

#include "UrchinCommon.h"

#include "resources/model/ConstAnimation.h"
#include "Meshes.h"

namespace urchin
{
	
	class Animation
	{
		public:
			Animation(const ConstAnimation *const, Meshes *const);
			~Animation();

			const Bone *getSkeleton() const;
			const AABBox<float> &getGlobalAABBox() const;
			const AABBox<float> &getGlobalLocalAABBox() const;

			void animate(float);

			void onMoving(const Transform<float> &);

		private:
			const ConstAnimation *const constAnimation;
			Meshes *const meshes;
			
			AnimationInformation animationInformation;
			Bone *const skeleton;
			AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
	};
	
}

#endif
