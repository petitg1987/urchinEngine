#include <utility>

#include <resources/model/ConstAnimation.h>
#include <resources/model/boundingbox/SplitBoundingBox.h>

namespace urchin {

    ConstAnimation::ConstAnimation(std::string animationFilename, unsigned int numFrames, unsigned int numBones, unsigned int frameRate,
                                   std::vector<std::vector<Bone>> skeletonFrames, std::vector<AABBox<float>> localFrameBBoxes) :
            animationFilename(std::move(animationFilename)),
            numBones(numBones),
            frameRate(frameRate),
            skeletonFrames(std::move(skeletonFrames)),
            localFrameBBoxes(std::move(localFrameBBoxes)) {
        localFramesBBox = AABBox<float>(this->localFrameBBoxes[0].getMin(), this->localFrameBBoxes[0].getMax());
        for (unsigned int i = 0; i < numFrames; ++i) {
            localFramesBBox = localFramesBBox.merge(this->localFrameBBoxes[i]);
        }
        SplitBoundingBox().split(localFramesBBox, localFramesSplitBBoxes);
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
        assert(skeletonFrames.size() > frameNumber);
        return skeletonFrames[frameNumber][boneNumber];
    }

    /**
     * @return Bounding box of the specified frame (not transformed)
     */
    const AABBox<float>& ConstAnimation::getLocalFrameAABBox(unsigned int frameNumber) const {
        assert(localFrameBBoxes.size() > frameNumber);
        return localFrameBBoxes[frameNumber];
    }

    /**
     * @return Bounding box regrouping all animation frames (not transformed)
     */
    const AABBox<float>& ConstAnimation::getLocalFramesAABBox() const {
        return localFramesBBox;
    }

    /**
     * @return Split bounding box for all animation frames (not transformed)
     */
    const std::vector<AABBox<float>>& ConstAnimation::getLocalFramesSplitAABBoxes() const {
        return localFramesSplitBBoxes;
    }

}
