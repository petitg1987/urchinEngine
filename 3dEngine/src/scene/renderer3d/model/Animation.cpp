#include "Animation.h"

namespace urchin
{

	Animation::Animation(ConstAnimation *constAnimation, Meshes *meshes) :
		constAnimation(constAnimation),
		meshes(meshes)
	{
		skeleton.resize(constAnimation->getNumberBones());

		animationInformation.currFrame = 0;
		animationInformation.nextFrame = 1;
		animationInformation.lastTime = 0;
		animationInformation.maxTime = 1.0f / constAnimation->getFrameRate();
	}

	Animation::~Animation()
	{
		constAnimation->release();
	}

	const std::vector<Bone> &Animation::getSkeleton() const
	{
		return skeleton;
	}

	const AABBox<float> &Animation::getGlobalAABBox() const
	{
		return globalBBox;
	}

	const std::vector<AABBox<float>> &Animation::getGlobalSplittedAABBox() const
	{
		return globalSplittedBBox;
	}

	/**
	 * @return Return global bounding box for all animations but not transformed
	 */
	const AABBox<float> &Animation::getGlobalLocalAABBox() const
	{
		return constAnimation->getOriginalGlobalAABBox();
	}

	const ConstAnimation *Animation::getConstAnimation() const
	{
		return constAnimation;
	}

	int Animation::getCurrFrame() const
	{
		return animationInformation.currFrame;
	}

	void Animation::onMoving(const Transform<float> &newTransform)
	{
		globalBBox = constAnimation->getOriginalGlobalAABBox().moveAABBox(newTransform);

		globalSplittedBBox.clear();
		const std::vector<AABBox<float>> &originalGlobalSplittedAABBox = constAnimation->getOriginalGlobalSplittedAABBox();
		for (const auto &originalGlobalSplitAABBox : originalGlobalSplittedAABBox)
		{
			globalSplittedBBox.push_back(originalGlobalSplitAABBox.moveAABBox(newTransform));
		}
	}

	void Animation::animate(float invFrameRate)
	{
		//calculate current and next frames
		animationInformation.lastTime += invFrameRate;
		if(animationInformation.lastTime >= animationInformation.maxTime) //move to next frame
		{
			animationInformation.lastTime = 0.0;
			animationInformation.currFrame = animationInformation.nextFrame;
			animationInformation.nextFrame++;

			if(animationInformation.nextFrame >= constAnimation->getNumberFrames())
			{
				animationInformation.nextFrame = 0;
			}
		}

		//interpolate skeletons between two frames
		float interp = animationInformation.lastTime * constAnimation->getFrameRate();
		for(unsigned int i = 0; i < constAnimation->getNumberBones(); ++i)
		{
			//copy parent index
			skeleton[i].parent = constAnimation->getBone(animationInformation.currFrame, i).parent;

			//linear interpolation for position
			skeleton[i].pos.X = constAnimation->getBone(animationInformation.currFrame, i).pos.X + interp * (constAnimation->getBone(animationInformation.nextFrame, i).pos.X - constAnimation->getBone(animationInformation.currFrame, i).pos.X);
			skeleton[i].pos.Y = constAnimation->getBone(animationInformation.currFrame, i).pos.Y + interp * (constAnimation->getBone(animationInformation.nextFrame, i).pos.Y - constAnimation->getBone(animationInformation.currFrame, i).pos.Y);
			skeleton[i].pos.Z = constAnimation->getBone(animationInformation.currFrame, i).pos.Z + interp * (constAnimation->getBone(animationInformation.nextFrame, i).pos.Z - constAnimation->getBone(animationInformation.currFrame, i).pos.Z);

			//spherical linear interpolation for orientation
			skeleton[i].orient = constAnimation->getBone(animationInformation.currFrame, i).orient.slerp(constAnimation->getBone(animationInformation.nextFrame, i).orient, interp);
		}

		//update the vertex and normals
		for(unsigned m=0; m<meshes->getNumberMeshes(); ++m)
		{
			meshes->getMesh(m)->update(skeleton);
		}
	}

}
