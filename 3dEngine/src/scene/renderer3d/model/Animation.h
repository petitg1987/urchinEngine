#ifndef URCHINENGINE_ANIMATION_H
#define URCHINENGINE_ANIMATION_H

#include <vector>
#include "UrchinCommon.h"

#include "resources/model/ConstAnimation.h"
#include "Meshes.h"

namespace urchin {

    class Animation {
        public:
            Animation(ConstAnimation *, Meshes*);
            ~Animation();

            const std::vector<Bone> &getSkeleton() const;
            const AABBox<float> &getGlobalAABBox() const;
            const std::vector<AABBox<float>> &getGlobalSplitAABBoxes() const;
            const AABBox<float> &getGlobalLocalAABBox() const;

            const ConstAnimation *getConstAnimation() const;
            unsigned int getCurrFrame() const;

            void animate(float);

            void onMoving(const Transform<float>&);

        private:
            mutable ConstAnimation *constAnimation;
            Meshes *meshes;

            AnimationInformation animationInformation;
            std::vector<Bone> skeleton;
            AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
            std::vector<AABBox<float>> globalSplitBBoxes;
    };

}

#endif
