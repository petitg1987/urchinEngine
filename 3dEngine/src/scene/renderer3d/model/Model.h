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
			Model(const Model &);
			~Model() override;
		
			void loadAnimation(const std::string &, const std::string &);
			void animate(const std::string &);
			void stopAnimation(bool);
			bool isAnimate() const;
		
			const ConstMeshes *getMeshes() const;
			std::map<std::string, const ConstAnimation *> getAnimations() const;

			const AABBox<float> &getAABBox() const override;
			std::vector<AABBox<float>> getSplitAABBoxes() const;
			const AABBox<float> &getLocalAABBox() const;
			
			void setPosition(const Point3<float> &);
			void setOrientation(const Quaternion<float> &);
			void setScale(float);
			void setTransform(const Transform<float> &);
			const Transform<float> &getTransform() const override;

			void setProduceShadow(bool);
			bool isProduceShadow() const;

			void updateAnimation(float);
			void display(const MeshParameter &) const;

			#ifdef _DEBUG
				void drawBaseBones(const Matrix4<float> &, const Matrix4<float> &) const;
			#endif

		private:
			void initialize(const std::string &);
			void onMoving(const Transform<float> &);

			static AABBox<float> defaultModelAABBox;

			//meshes
			Meshes *meshes;
			
			//animations
			std::map<std::string, Animation *> animations;
			Animation *currAnimation;
			bool stopAnimationAtLastFrame;
		
			//transform
			Transform<float> transform;

			//properties
			bool bIsProduceShadow;
	};

}

#endif
