#include "Animation.h"

namespace urchin {

    Animation::Animation(ConstAnimation* constAnimation, Meshes* meshes) :
        constAnimation(constAnimation),
        meshes(meshes),
        animationInformation() {
        skeleton.resize(constAnimation->getNumberBones());

        animationInformation.currFrame = 0;
        animationInformation.nextFrame = 1;
        animationInformation.lastTime = 0;
        animationInformation.maxTime = 1.0f / static_cast<float>(constAnimation->getFrameRate());
    }

    Animation::~Animation() {
        constAnimation->release();
    }

    const std::vector<Bone> &Animation::getSkeleton() const {
        return skeleton;
    }

    const AABBox<float> &Animation::getGlobalAABBox() const {
        return globalBBox;
    }

    const std::vector<AABBox<float>> &Animation::getGlobalSplitAABBoxes() const {
        return globalSplitBBoxes;
    }

    /**
     * @return Return global bounding box for all animations but not transformed
     */
    const AABBox<float> &Animation::getGlobalLocalAABBox() const {
        return constAnimation->getOriginalGlobalAABBox();
    }

    const ConstAnimation *Animation::getConstAnimation() const {
        return constAnimation;
    }

    unsigned int Animation::getCurrFrame() const {
        return animationInformation.currFrame;
    }

    void Animation::onMoving(const Transform<float>& newTransform) {
        globalBBox = constAnimation->getOriginalGlobalAABBox().moveAABBox(newTransform);

        globalSplitBBoxes.clear();
        const std::vector<AABBox<float>>& originalGlobalSplitAABBoxes = constAnimation->getOriginalGlobalSplitAABBoxes();
        for (const auto& originalGlobalSplitAABBox : originalGlobalSplitAABBoxes) {
            globalSplitBBoxes.push_back(originalGlobalSplitAABBox.moveAABBox(newTransform));
        }
    }

    void Animation::animate(float dt) {
        //calculate current and next frames
        animationInformation.lastTime += dt;
        if (animationInformation.lastTime >= animationInformation.maxTime) //move to next frame
        {
            animationInformation.lastTime = 0.0f;
            animationInformation.currFrame = animationInformation.nextFrame;
            animationInformation.nextFrame++;

            if (animationInformation.nextFrame >= constAnimation->getNumberFrames()) {
                animationInformation.nextFrame = 0;
            }
        }

        //interpolate skeletons between two frames
        float interp = animationInformation.lastTime * static_cast<float>(constAnimation->getFrameRate());
        for (std::size_t i = 0; i < constAnimation->getNumberBones(); ++i) {
            //shortcut
            const Bone& currentFrameBone = constAnimation->getBone(animationInformation.currFrame, i);
            const Bone& nextFrameBone = constAnimation->getBone(animationInformation.nextFrame, i);

            //copy parent index
            skeleton[i].parent = currentFrameBone.parent;

            //linear interpolation for position
            skeleton[i].pos.X = currentFrameBone.pos.X + interp * (nextFrameBone.pos.X - currentFrameBone.pos.X);
            skeleton[i].pos.Y = currentFrameBone.pos.Y + interp * (nextFrameBone.pos.Y - currentFrameBone.pos.Y);
            skeleton[i].pos.Z = currentFrameBone.pos.Z + interp * (nextFrameBone.pos.Z - currentFrameBone.pos.Z);

            //spherical linear interpolation for orientation
            skeleton[i].orient = currentFrameBone.orient.slerp(nextFrameBone.orient, interp);
        }

        //update the vertex and normals
        for (unsigned m=0; m<meshes->getNumberMeshes(); ++m) {
            meshes->getMesh(m)->update(skeleton);
        }
    }

}
