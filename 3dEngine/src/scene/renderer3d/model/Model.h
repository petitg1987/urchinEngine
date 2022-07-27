#pragma once

#include <string>
#include <map>
#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Meshes.h>
#include <scene/renderer3d/model/Animation.h>
#include <scene/renderer3d/model/AnimRepeat.h>
#include <scene/renderer3d/model/AnimStart.h>
#include <scene/renderer3d/model/displayer/ModelDisplayable.h>
#include <resources/model/ConstMeshes.h>
#include <resources/model/ConstAnimation.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class Model : public Octreeable<Model>, public ModelDisplayable {
        public:
            enum NotificationType {
                MESH_VERTICES_UPDATED = Octreeable::MAX_NOTIFICATION_TYPE,
                MESH_UV_UPDATED,
                MATERIAL_UPDATED,
                SCALE_UPDATED,
                ANIMATION_STARTED,
                ANIMATION_ENDED,
                CULL_BEHAVIOR_UPDATED
            };
            enum class ShadowBehavior {
                RECEIVER_AND_CASTER, //Models which can receive and cast shadow
                RECEIVER_ONLY, //Models which only receive shadow (e.g.: ground). Can provide better shadow quality.
                NONE //Models which does not receive and cast shadow. Can improve performance.
            };
            enum class CullBehavior {
                CULL,
                NO_CULL
            };

            Model(const Model&);
            Model& operator=(const Model&) = delete;
            ~Model() override = default;
            static std::unique_ptr<Model> fromMeshesFile(const std::string&);
            static std::unique_ptr<Model> fromMemory(std::unique_ptr<Meshes>);

            void loadAnimation(const std::string&, const std::string&);
            bool hasAnimationLoaded(std::string_view) const;
            void animate(std::string_view, AnimRepeat, AnimStart);
            void stopAnimation(bool);
            void resetAnimations();
            void gotoAnimationFrame(std::string_view, unsigned int);
            const Animation* getActiveAnimation() const;
            bool hasActiveAnimation() const;
            bool isAnimated() const;

            std::string getName() const override;
            const Meshes* getMeshes() const;
            const ConstMeshes* getConstMeshes() const;
            const std::map<std::string, std::unique_ptr<Animation>, std::less<>>& getAnimations() const;

            const AABBox<float>& getAABBox() const override;
            const std::vector<AABBox<float>>& getSplitAABBoxes() const;
            const AABBox<float>& getLocalAABBox() const;

            void setPosition(const Point3<float>&);
            void setOrientation(const Quaternion<float>&);
            void setScale(const Vector3<float>&);
            void setTransform(const Transform<float>&);
            const Transform<float>& getTransform() const override;

            void setShadowBehavior(ShadowBehavior);
            ShadowBehavior getShadowBehavior() const;
            void setCullBehavior(CullBehavior);
            CullBehavior getCullBehavior() const;
            bool isOriginalMeshesUpdated() const;
            bool isMeshUpdated(unsigned int) const;

            void updateAnimation(float);
            void updateVertices(unsigned int, const std::vector<Point3<float>>&);
            void updateUv(unsigned int, const std::vector<Point2<float>>&);
            void updateMaterial(unsigned int, std::shared_ptr<Material>);

        private:
            explicit Model(const std::string&);
            explicit Model(std::unique_ptr<Meshes>);

            const AABBox<float> &getDefaultModelLocalAABBox() const;
            void initialize();
            void onMoving(const Transform<float>&);
            void notifyMeshVerticesUpdatedByAnimation();
            void notifyMeshVerticesUpdated();
            void notifyMeshVerticesUpdated(unsigned int);
            void notifyMeshUvUpdated(unsigned int);
            void notifyMeshMaterialUpdated(unsigned int);

            std::vector<AABBox<float>> defaultModelAABBoxes;

            //meshes
            std::unique_ptr<Meshes> meshes;

            //animations
            std::map<std::string, std::unique_ptr<Animation>, std::less<>> animations;
            Animation* activeAnimation;
            bool isModelAnimated;
            bool stopAnimationAtLastFrame;

            //transform
            Transform<float> transform;

            //properties
            ShadowBehavior shadowBehavior;
            CullBehavior cullBehavior;
            std::vector<bool> meshesUpdated;
            bool originalMeshesUpdated;
    };

}
