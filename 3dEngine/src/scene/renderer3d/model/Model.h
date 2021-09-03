#pragma once

#include <string>
#include <map>
#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Meshes.h>
#include <scene/renderer3d/model/Animation.h>
#include <resources/model/ConstMeshes.h>
#include <resources/model/ConstAnimation.h>
#include <graphic/render/target/RenderTarget.h>

namespace urchin {

    class Model : public Octreeable<Model> {
        public:
            enum NotificationType {
                MESH_UPDATED = Octreeable::MAX_NOTIFICATION_TYPE
            };

            explicit Model(const std::string&);
            explicit Model(std::unique_ptr<Meshes>);
            Model(const Model&);

            void loadAnimation(const std::string&, const std::string&);
            void animate(const std::string&);
            void stopAnimation(bool);
            bool isAnimate() const;

            const Meshes* getMeshes() const;
            const ConstMeshes* getConstMeshes() const;
            std::map<std::string, const ConstAnimation*> getAnimations() const;

            const AABBox<float>& getAABBox() const override;
            const std::vector<AABBox<float>>& getSplitAABBoxes() const;
            const AABBox<float>& getLocalAABBox() const;

            void setPosition(const Point3<float>&);
            void setOrientation(const Quaternion<float>&);
            void setScale(float);
            void setTransform(const Transform<float>&);
            const Transform<float>& getTransform() const override;

            void setProduceShadow(bool);
            bool isProduceShadow() const;

            void updateAnimation(float);

        private:
            const AABBox<float> &getDefaultModelLocalAABBox() const;
            void initialize(const std::string&);
            void onMoving(const Transform<float>&);

            std::vector<AABBox<float>> defaultModelAABBoxes;

            //meshes
            std::unique_ptr<Meshes> meshes;

            //animations
            std::map<std::string, std::unique_ptr<Animation>> animations;
            Animation* currAnimation;
            bool stopAnimationAtLastFrame;

            //transform
            Transform<float> transform;

            //properties
            bool bIsProduceShadow;
    };

}
