#include <scene/renderer3d/model/Animation.h>

namespace urchin {

    Animation::Animation(std::shared_ptr<ConstAnimation> constAnimation, const Meshes& meshes, AnimShadowImpact animShadowImpact) :
            constAnimation(std::move(constAnimation)),
            meshes(meshes),
            animShadowImpact(animShadowImpact),
            animationInformation() {
        skeleton.resize(this->constAnimation->getNumberBones());

        for (unsigned int meshIndex = 0; meshIndex < meshes.getNumberMeshes(); ++meshIndex) {
            for (std::size_t boneIndex : meshes.getConstMeshes().getConstMesh(meshIndex).getUsedBoneIndices()) {
                if (this->constAnimation->isAnimatedBone(boneIndex) && std::ranges::find(animatedMeshIndices, meshIndex) == animatedMeshIndices.end()) {
                    animatedMeshIndices.push_back(meshIndex);
                }
            }
        }

        animationInformation.lastTime = 0.0f;
        animationInformation.maxTime = 1.0f / (float)this->constAnimation->getFrameRate();
        animationInformation.currFrame = 0;
        computeNextFrame();
    }

    const std::vector<Bone>& Animation::getSkeleton() const {
        return skeleton;
    }

    /**
     * @return Bounding box regrouping all animation frames (transformed by the model matrix)
     */
    const AABBox<float>& Animation::getFramesAABBox() const {
        return framesBBox;
    }

    /**
     * @return Split bounding box regrouping all animation frames (transformed by the model matrix)
     */
    const std::vector<AABBox<float>>& Animation::getFramesSplitAABBoxes() const {
        return framesSplitBBoxes;
    }

    /**
     * @return Bounding box regrouping all animation frames (not transformed)
     */
    const AABBox<float>& Animation::getLocalFramesAABBox() const {
        return constAnimation->getLocalFramesAABBox();
    }

    const ConstAnimation& Animation::getConstAnimation() const {
        return *constAnimation;
    }

    AnimShadowImpact Animation::getShadowImpact() const {
        return animShadowImpact;
    }

    unsigned int Animation::getCurrentFrame() const {
        return animationInformation.currFrame;
    }

    float Animation::getAnimationProgression() const {
        auto currentFrame = (float)getCurrentFrame();
        currentFrame += animationInformation.lastTime / animationInformation.maxTime;
        return currentFrame / ((float)getConstAnimation().getNumberFrames() - 1.0f);
    }

    const std::vector<std::size_t>& Animation::getAnimatedMeshIndices() const {
        return animatedMeshIndices;
    }

    void Animation::onMoving(const Transform<float>& newTransform) {
        framesBBox = constAnimation->getLocalFramesAABBox().moveAABBox(newTransform);

        framesSplitBBoxes.clear();
        for (const auto& localFramesSplitAABBox : constAnimation->getLocalFramesSplitAABBoxes()) {
            framesSplitBBoxes.push_back(localFramesSplitAABBox.moveAABBox(newTransform));
        }
    }

    void Animation::animate(float dt) {
        //calculate current and next frames
        animationInformation.lastTime += dt;
        if (animationInformation.lastTime >= animationInformation.maxTime) { //move to next frame
            animationInformation.lastTime = 0.0f;
            animationInformation.currFrame = animationInformation.nextFrame;
            computeNextFrame();
        }

        //interpolate skeletons between two frames
        float interp = animationInformation.lastTime * (float)constAnimation->getFrameRate();
        for (unsigned int i = 0; i < constAnimation->getNumberBones(); ++i) {
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

        //update the mesh (vertex, normals...)
        for (unsigned int meshIndex = 0; meshIndex < meshes.getNumberMeshes(); ++meshIndex) {
            meshes.getMesh(meshIndex).updateSkeleton(skeleton);
        }
    }

    void Animation::computeNextFrame() {
        animationInformation.nextFrame = animationInformation.currFrame + 1;
        if (animationInformation.nextFrame >= constAnimation->getNumberFrames()) {
            animationInformation.nextFrame = 0;
        }
    }

    void Animation::gotoFrame(unsigned int frame) {
        if (frame >= constAnimation->getNumberFrames()) {
            throw std::runtime_error("Frame (" + std::to_string(frame) + ") does not exist in animation " + constAnimation->getName() + ". Total frame: " + std::to_string(constAnimation->getNumberFrames()));
        } else if (frame == getCurrentFrame()) {
            return;
        }

        //update animation information
        animationInformation.lastTime = 0.0f;
        animationInformation.currFrame = frame;
        computeNextFrame();

        //update skeletons
        for (unsigned int i = 0; i < constAnimation->getNumberBones(); ++i) {
            const Bone& frameBone = constAnimation->getBone(frame, i);

            skeleton[i].parent = frameBone.parent;
            skeleton[i].pos = frameBone.pos;
            skeleton[i].orient = frameBone.orient;
        }

        //update the mesh (vertex, normals...)
        for (unsigned int meshIndex = 0; meshIndex < meshes.getNumberMeshes(); ++meshIndex) {
            meshes.getMesh(meshIndex).updateSkeleton(skeleton);
        }
    }

}
