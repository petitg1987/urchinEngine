#include <utility>

#include <resources/model/ConstAnimation.h>
#include <resources/model/boundingbox/SplitBoundingBox.h>

namespace urchin {

    ConstAnimation::ConstAnimation(std::string animationFilename, unsigned int numFrames, unsigned int numBones, unsigned int frameRate,
                                   std::vector<std::vector<Bone>> skeletonFrames, std::vector<std::unique_ptr<AABBox<float>>> bboxes) :
            animationFilename(std::move(animationFilename)),
            numBones(numBones),
            frameRate(frameRate),
            skeletonFrames(std::move(skeletonFrames)) {
        //determines the bounding box (bboxes: bounding boxes of each animation frames (not transformed))
        originalGlobalBBox = AABBox<float>(bboxes[0]->getMin(), bboxes[0]->getMax());
        for (unsigned int i = 0; i < numFrames; ++i) {
            originalGlobalBBox = originalGlobalBBox.merge(*bboxes[i]);
        }
        SplitBoundingBox().split(originalGlobalBBox, originalGlobalSplitBBoxes);
    }

    const std::string& ConstAnimation::getAnimationFilename() const {
        return animationFilename;
    }

    unsigned int ConstAnimation::getNumberFrames() const {
        return (unsigned int)skeletonFrames.size();
    }

    unsigned int ConstAnimation::getNumberBones() const {
        return numBones;
    }

    unsigned int ConstAnimation::getFrameRate() const {
        return frameRate;
    }

    const Bone& ConstAnimation::getBone(unsigned int frameNumber, unsigned int boneNumber) const {
        return skeletonFrames[frameNumber][boneNumber];
    }

    const AABBox<float>& ConstAnimation::getOriginalGlobalAABBox() const {
        return originalGlobalBBox;
    }

    const std::vector<AABBox<float>>& ConstAnimation::getOriginalGlobalSplitAABBoxes() const {
        return originalGlobalSplitBBoxes;
    }

}
