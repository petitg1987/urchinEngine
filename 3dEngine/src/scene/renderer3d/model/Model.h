#pragma once

#include <string>
#include <map>
#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Meshes.h>
#include <scene/renderer3d/model/Animation.h>
#include <resources/model/ConstMeshes.h>
#include <resources/model/ConstAnimation.h>
#include <api/render/target/RenderTarget.h>

namespace urchin {

    class Model : public Octreeable<Model> {
        public:
            enum NotificationType {
                MESH_UPDATED = Octreeable::MAX_NOTIFICATION_TYPE,
                MATERIAL_UPDATED,
                SCALE_UPDATED
            };

            Model(const Model&);
            static std::unique_ptr<Model> fromMeshesFile(const std::string&);
            static std::unique_ptr<Model> fromMemory(std::unique_ptr<Meshes>);

            void loadAnimation(const std::string&, const std::string&);
            void animate(const std::string&, bool);
            void stopAnimation(bool);
            void resetAnimations();
            void gotoAnimationFrame(const std::string&, unsigned int);
            const Animation* getActiveAnimation() const;
            bool hasActiveAnimation() const;
            bool isAnimated() const;

            const Meshes* getMeshes() const;
            const ConstMeshes* getConstMeshes() const;
            std::map<std::string, const ConstAnimation*> getAnimations() const;

            const AABBox<float>& getAABBox() const override;
            const std::vector<AABBox<float>>& getSplitAABBoxes() const;
            const AABBox<float>& getLocalAABBox() const;

            void setPosition(const Point3<float>&);
            void setOrientation(const Quaternion<float>&);
            void setScale(const Vector3<float>&);
            void setTransform(const Transform<float>&);
            const Transform<float>& getTransform() const override;

            void setProduceShadow(bool);
            bool isProduceShadow() const;

            void updateAnimation(float);
            void updateMesh(unsigned int, const std::vector<Point3<float>>&);
            void updateMaterial(unsigned int, const std::shared_ptr<Material>&);

        private:
            explicit Model(const std::string&);
            explicit Model(std::unique_ptr<Meshes>);

            const AABBox<float> &getDefaultModelLocalAABBox() const;
            void initialize();
            void onMoving(const Transform<float>&);

            std::vector<AABBox<float>> defaultModelAABBoxes;

            //meshes
            std::unique_ptr<Meshes> meshes;

            //animations
            std::map<std::string, std::unique_ptr<Animation>> animations;
            Animation* activeAnimation;
            bool isModelAnimated;
            bool stopAnimationAtLastFrame;

            //transform
            Transform<float> transform;

            //properties
            bool bIsProduceShadow;
    };

}
