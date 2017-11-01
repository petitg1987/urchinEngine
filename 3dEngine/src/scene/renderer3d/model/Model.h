#ifndef URCHINENGINE_MODEL_H
#define URCHINENGINE_MODEL_H

#include <string>
#include <map>
#include <vector>
#include "UrchinCommon.h"

#include "Meshes.h"
#include "Animation.h"
#include "scene/renderer3d/octree/Octreeable.h"
#include "resources/model/ConstMeshes.h"
#include "resources/model/ConstAnimation.h"

namespace urchin
{

	class Model : public Octreeable<Model>
	{
		public:
			explicit Model(const std::string &);
			~Model() override;
		
			void loadAnimation(const std::string &, const std::string &);
			void animate(const std::string &);
			bool isAnimate() const;
		
			const ConstMeshes *getMeshes() const;
			std::map<std::string, const ConstAnimation *> getAnimations() const;

			const AABBox<float> &getAABBox() const override;
			const std::vector<AABBox<float>> &getSplittedAABBox() const;
			const AABBox<float> &getLocalAABBox() const;
			
			void setPosition(const Point3<float> &);
			void setOrientation(const Quaternion<float> &);
			void setScale(float);
			void setTransform(const Transform<float> &);
			const Transform<float> &getTransform() const override;

			void setProduceShadow(bool);
			bool isProduceShadow() const;

			void updateAnimation(float);
			void display(const MeshParameter &);

		private:
			void onMoving(const Transform<float> &);

			//meshes
			ConstMeshes *constMeshes; //constant part of the meshes (common to all cloned models)
			Meshes *meshes; //variable part of the meshes
			
			//animations
			std::map<std::string, ConstAnimation *> constAnimations; //constant part of the animations (common to all cloned models)
			std::map<std::string, Animation *> animations; //variable part of the animations
			const ConstAnimation *currConstAnimation;
			Animation *currAnimation;
		
			//transform
			Transform<float> transform;

			//properties
			bool bIsProduceShadow;
	};

}

#endif
