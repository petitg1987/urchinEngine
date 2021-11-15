#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/model/ConstAnimation.h>
#include <scene/renderer3d/model/Meshes.h>

namespace urchin {

    class Animation {
        public:
            Animation(std::shared_ptr<ConstAnimation>, Meshes&);

            const std::vector<Bone>& getSkeleton() const;
            const AABBox<float>& getGlobalAABBox() const;
            const std::vector<AABBox<float>>& getGlobalSplitAABBoxes() const;
            const AABBox<float>& getGlobalLocalAABBox() const;

            const ConstAnimation& getConstAnimation() const;
            unsigned int getCurrFrame() const;

            void animate(float);
            void gotoFrame(unsigned int);

            void onMoving(const Transform<float>&);

        private:
            void computeNextFrame();

            mutable std::shared_ptr<ConstAnimation> constAnimation;
            Meshes& meshes;

            AnimationInformation animationInformation;
            std::vector<Bone> skeleton;
            AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
            std::vector<AABBox<float>> globalSplitBBoxes;
    };

}
