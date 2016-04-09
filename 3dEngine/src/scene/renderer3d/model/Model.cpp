#include <GL/glew.h>
#include <GL/gl.h>
#include <limits>
#include <stdexcept>

#include "Model.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{
	
	Model::Model(const std::string &meshFilename) :
			currConstAnimation(nullptr),
			currAnimation(nullptr),
			bIsProduceShadow(true)
	{
		constMeshes = MediaManager::instance()->getMedia<ConstMeshes>(meshFilename);
		meshes = new Meshes(constMeshes);
		meshes->onMoving(transform);
	}

	Model::~Model()
	{
		constMeshes->release();
		delete meshes;
		
		for(std::map<std::string, Animation*>::iterator it_anims=animations.begin(); it_anims!=animations.end(); ++it_anims)
		{
			delete (*it_anims).second;
		}
		
		for(std::map<std::string, ConstAnimation *>::iterator it_anims=constAnimations.begin(); it_anims!=constAnimations.end(); ++it_anims)
		{
			((*it_anims).second)->release();
		}
	}

	void Model::loadAnimation(const std::string &name, const std::string &filename)
	{
		//load and add the anim to the std::map
		ConstAnimation *constAnimation = MediaManager::instance()->getMedia<ConstAnimation>(filename);
		constAnimations[name] = constAnimation;
		animations[name] = new Animation(constAnimation, meshes);
		animations[name]->onMoving(transform);

		//both files must have the same number of bones
		if(constMeshes->getConstMesh(0)->getNumberBones() != constAnimation->getNumberBones())
		{
			throw std::runtime_error("Both files haven't the same number of bones. Meshes filename: " + constMeshes->getName() + ", Animation filename: " + constAnimation->getName() + ".");
		}

		//we just check with mesh[0] && frame[0]
		for(unsigned int i = 0; i<constMeshes->getConstMesh(0)->getNumberBones(); ++i)
		{
			//bones must have the same parent index
			if(constMeshes->getConstMesh(0)->getBaseBone(i).parent != constAnimation->getBone(0, i).parent)
			{
				throw std::runtime_error("Bones haven't the same parent index. Meshes filename: " + constMeshes->getName() + ", Animation filename: " + constAnimation->getName() + ".");
			}

			//bones must have the same name
			if(constMeshes->getConstMesh(0)->getBaseBone(i).name.compare(constAnimation->getBone(0, i).name))
			{
				throw std::runtime_error("Bones haven't the same name. Meshes filename: " + constMeshes->getName() + ", Animation filename: " + constAnimation->getName() + ".");
			}
		}
	}

	void Model::animate(const std::string &animationName)
	{
		if(animationName.empty())
		{
			currConstAnimation = nullptr;
			currAnimation = nullptr;
		}else
		{
			currConstAnimation = constAnimations[animationName];
			currAnimation = animations[animationName];
		}
		
		onMoving(transform);
	}

	bool Model::isAnimate() const
	{
		return currAnimation!=nullptr && currConstAnimation!=nullptr;
	}

	void Model::onMoving(const Transform<float> &newTransform)
	{
		//inform the OctreeManager that the model should be updated in the octree
		this->notifyOctreeableMove();

		//update the bounding box
		meshes->onMoving(newTransform);
		if(currAnimation)
		{
			currAnimation->onMoving(newTransform);
		}
	}

	const ConstMeshes *Model::getMeshes() const
	{
		return constMeshes;
	}

	std::map<std::string, const ConstAnimation *> Model::getAnimations() const
	{
		std::map<std::string, const ConstAnimation *> constConstAnimations;
		constConstAnimations.insert(constAnimations.begin(), constAnimations.end());

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
		}else
		{
			return meshes->getGlobalAABBox();
		}
	}

	/**
	 * @return identical to getAABBox() method but the bounding box is splitted to the limit size configured
	 */
	const std::vector<AABBox<float>> &Model::getSplittedAABBox() const
	{
		if(isAnimate())
		{
			return currAnimation->getGlobalSplittedAABBox();
		}else
		{
			return meshes->getGlobalSplittedAABBox();
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
		}else
		{
			return meshes->getGlobalLocalAABBox();
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
	void Model::setProduceShadow(bool bIsProduceShaodw)
	{
		this->bIsProduceShadow = bIsProduceShaodw;
	}

	bool Model::isProduceShadow() const
	{
		return bIsProduceShadow;
	}

	void Model::updateAnimation(float invFrameRate)
	{
		//animate model
		if(isAnimate())
		{
			currAnimation->animate(invFrameRate);
		}
	}

	void Model::display(const MeshParameter &meshParameter)
	{
		for(unsigned int m=0; m<meshes->getNumberMeshes(); ++m)
		{
			meshes->getMesh(m)->display(meshParameter);
		}
	}

}
