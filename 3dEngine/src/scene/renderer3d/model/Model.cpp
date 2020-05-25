#include <stdexcept>

#include "Model.h"
#include "resources/MediaManager.h"
#include "utils/display/geometry/aabbox/AABBoxModel.h"

namespace urchin
{

    //static
    AABBox<float> Model::defaultModelLocalAABBox = AABBox<float>(Point3<float>(-0.5f, -0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f));

	Model::Model(const std::string &meshFilename) :
            defaultModelAABBoxes({defaultModelLocalAABBox}),
			meshes(nullptr),
			currAnimation(nullptr),
			stopAnimationAtLastFrame(false),
			bIsProduceShadow(true)
	{
        initialize(meshFilename);
	}

    Model::Model(const Model &model) : Octreeable(model),
            defaultModelAABBoxes({defaultModelLocalAABBox}),
            meshes(nullptr),
			currAnimation(nullptr),
			stopAnimationAtLastFrame(false),
            bIsProduceShadow(true)
    {
        std::string meshFilename = model.getMeshes()!=nullptr ? model.getMeshes()->getName() : "";
        initialize(meshFilename);

        setTransform(model.getTransform());
        setProduceShadow(model.isProduceShadow());
    }

	Model::~Model()
	{
		delete meshes;
		
		for (auto &animation : animations)
		{
			delete animation.second;
		}
	}

    void Model::initialize(const std::string &meshFilename)
    {
        if(!meshFilename.empty())
        {
			auto *constMeshes = MediaManager::instance()->getMedia<ConstMeshes>(meshFilename);
            meshes = new Meshes(constMeshes);
            meshes->onMoving(transform);
        }
    }


	void Model::loadAnimation(const std::string &name, const std::string &filename)
	{
		if(!meshes)
		{
			throw std::runtime_error("Cannot add animation on model without mesh");
		}

		//load and add the anim to the std::map
		auto *constAnimation = MediaManager::instance()->getMedia<ConstAnimation>(filename);
		animations[name] = new Animation(constAnimation, meshes);
		animations[name]->onMoving(transform);

		//both files must have the same number of bones
		if(meshes->getConstMeshes()->getConstMesh(0)->getNumberBones() != constAnimation->getNumberBones())
		{
			throw std::runtime_error("Both files haven't the same number of bones. Meshes filename: " + meshes->getConstMeshes()->getName() + ", Animation filename: " + constAnimation->getName() + ".");
		}

		//we just check with mesh[0] && frame[0]
		for(std::size_t i = 0; i<meshes->getConstMeshes()->getConstMesh(0)->getNumberBones(); ++i)
		{
			//bones must have the same parent index
			if(meshes->getConstMeshes()->getConstMesh(0)->getBaseBone(i).parent != constAnimation->getBone(0, i).parent)
			{
				throw std::runtime_error("Bones haven't the same parent index. Meshes filename: " + meshes->getConstMeshes()->getName() + ", Animation filename: " + constAnimation->getName() + ".");
			}

			//bones must have the same name
			if(meshes->getConstMeshes()->getConstMesh(0)->getBaseBone(i).name != constAnimation->getBone(0, i).name)
			{
				throw std::runtime_error("Bones haven't the same name. Meshes filename: " + meshes->getConstMeshes()->getName() + ", Animation filename: " + constAnimation->getName() + ".");
			}
		}
	}

	void Model::animate(const std::string &animationName)
	{
		currAnimation = animations.at(animationName);

		onMoving(transform);
	}

	void Model::stopAnimation(bool immediate)
	{
		if (immediate)
		{
			currAnimation = nullptr;
		}else if(isAnimate())
		{
			stopAnimationAtLastFrame = true;
		}

		onMoving(transform);
	}

	bool Model::isAnimate() const
	{
		return currAnimation != nullptr;
	}

	void Model::onMoving(const Transform<float> &newTransform)
	{
		//update the bounding box
		if(meshes)
		{
			meshes->onMoving(newTransform);
            if(currAnimation)
            {
                currAnimation->onMoving(newTransform);
            }
		}else
        {
            defaultModelAABBoxes[0] = defaultModelLocalAABBox.moveAABBox(transform);
        }

        //inform the OctreeManager that the model should be updated in the octree
        this->notifyOctreeableMove();
    }

	const ConstMeshes *Model::getMeshes() const
	{
	    if(meshes)
        {
            return meshes->getConstMeshes();
        }

        return nullptr;
	}

	std::map<std::string, const ConstAnimation *> Model::getAnimations() const
	{
		std::map<std::string, const ConstAnimation *> constConstAnimations;
		for(const auto &animation : animations)
		{
			constConstAnimations.insert(std::pair<std::string, const ConstAnimation *>(animation.first, animation.second->getConstAnimation()));
		}
		return constConstAnimations;
	}

	/**
	* @return Merged bounding box for all animations. If not animation exist: return meshes bounding box.
	*/
	const AABBox<float> &Model::getAABBox() const
	{
		if(isAnimate())
		{
			return currAnimation->getGlobalAABBox();
		}else if(meshes)
		{
			return meshes->getGlobalAABBox();
		}else
        {
            return defaultModelAABBoxes[0];
        }
	}

	/**
	 * @return identical to getAABBox() method but the bounding box is split to the limit size configured
	 */
	const std::vector<AABBox<float>> &Model::getSplitAABBoxes() const
	{
		if(isAnimate())
		{
			return currAnimation->getGlobalSplitAABBoxes();
		}else if(meshes)
		{
			return meshes->getGlobalSplitAABBoxes();
		}else
        {
            return defaultModelAABBoxes;
        }
	}

	/**
	* @return Local merged bounding box for all animations. If not animation exist: return local meshes bounding box.
	*/
	const AABBox<float> &Model::getLocalAABBox() const
	{
		if(isAnimate())
		{
			return currAnimation->getGlobalLocalAABBox();
		}else if(meshes)
		{
			return meshes->getGlobalLocalAABBox();
		}else
        {
            return defaultModelLocalAABBox;
        }
	}

	void Model::setPosition(const Point3<float> &p)
	{
		transform.setPosition(p);
		onMoving(transform);
	}

	void Model::setOrientation(const Quaternion<float> &q)
	{
		transform.setOrientation(q);
		onMoving(transform);
	}

	void Model::setScale(float scale)
	{
		transform.setScale(scale);
		onMoving(transform);
	}

	void Model::setTransform(const Transform<float> &transform)
	{
		this->transform = transform;
		onMoving(transform);
	}

	const Transform<float> &Model::getTransform() const
	{
		return transform;
	}

	/**
	 * @param isProduceShadow Indicate whether model can produce shadow. The value should be generally set to false for ground to having better shadow rendering.
	 */
	void Model::setProduceShadow(bool bIsProduceShadow)
	{
		this->bIsProduceShadow = bIsProduceShadow;
	}

	bool Model::isProduceShadow() const
	{
		return bIsProduceShadow;
	}

	void Model::updateAnimation(float dt)
	{
		//animate model
		if(isAnimate())
		{
			if(stopAnimationAtLastFrame && currAnimation->getCurrFrame() == 0)
			{
				stopAnimation(true);
				stopAnimationAtLastFrame = false;
			}else
            {
                currAnimation->animate(dt);
            }
		}
	}

	void Model::display(const MeshParameter &meshParameter) const
	{
        if(meshes)
        {
            for (unsigned int m = 0; m < meshes->getNumberMeshes(); ++m)
            {
                meshes->getMesh(m)->display(meshParameter);
            }
        }
	}

    void Model::drawBBox(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
    {
        AABBoxModel aabboxModel(getAABBox());
        aabboxModel.onCameraProjectionUpdate(projectionMatrix);
        aabboxModel.display(viewMatrix);
    }

	void Model::drawBaseBones(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		if(meshes)
		{
			for (unsigned int m = 0; m < meshes->getNumberMeshes(); ++m)
			{
				meshes->getMesh(m)->drawBaseBones(projectionMatrix, viewMatrix * getTransform().getTransformMatrix());
			}
		}
	}

}
