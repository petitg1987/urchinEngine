#pragma once

#include <UrchinCommon.h>

#include <resources/Resource.h>
#include <resources/model/ConstMesh.h>

namespace urchin {

    struct AnimationInformation {
        float lastTime;
        float maxTime;
        unsigned int currFrame;
        unsigned int nextFrame;
    };

    /**
     * Contains all the constant/common data for an animation.
     * Two identical models can use the instance of this class.
     */
    class ConstAnimation : public Resource {
        public:
            ConstAnimation(std::string , unsigned int, unsigned int, unsigned int, std::vector<std::vector<Bone>>, std::vector<std::unique_ptr<AABBox<float>>>);
            ~ConstAnimation() override = default;

            const std::string& getAnimationFilename() const;
            unsigned int getNumberFrames() const;
            unsigned int getNumberBones() const;
            unsigned int getFrameRate() const;
            const Bone& getBone(unsigned int, unsigned int) const;

            const AABBox<float>& getOriginalGlobalAABBox() const;
            const std::vector<AABBox<float>>& getOriginalGlobalSplitAABBoxes() const;

        private:
            std::string animationFilename;
            const unsigned int numBones, frameRate;
            std::vector<std::vector<Bone>> skeletonFrames;
            AABBox<float> originalGlobalBBox; //original global bounding box (not transformed)
            std::vector<AABBox<float>> originalGlobalSplitBBoxes;
    };

}
