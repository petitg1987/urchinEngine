#include <stdexcept>

#include <scene/renderer3d/model/Model.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Model::Model(const std::string& meshesFilename) :
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            bIsProduceShadow(true),
            bIsMeshUpdated(false) {
        if (!meshesFilename.empty()) {
            auto constMeshes = ResourceRetriever::instance().getResource<ConstMeshes>(meshesFilename);
            meshes = std::make_unique<Meshes>(std::move(constMeshes));
        }
        initialize();
    }

    Model::Model(std::unique_ptr<Meshes> meshes) :
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            meshes(std::move(meshes)),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            bIsProduceShadow(true),
            bIsMeshUpdated(false) {
        initialize();
    }

    Model::Model(const Model& model) :
            Octreeable(model),
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            transform(model.getTransform()),
            bIsProduceShadow(model.isProduceShadow()),
            bIsMeshUpdated(model.isMeshUpdated()) {
        if (model.meshes) {
            meshes = std::make_unique<Meshes>(model.meshes->copyConstMeshesRef());
        }
        initialize();
    }

    std::unique_ptr<Model> Model::fromMeshesFile(const std::string& meshesFilename) {
        return std::unique_ptr<Model>(new Model(meshesFilename));
    }

    std::unique_ptr<Model> Model::fromMemory(std::unique_ptr<Meshes> meshes) {
        return std::unique_ptr<Model>(new Model(std::move(meshes)));
    }

    void Model::initialize() {
        if (meshes) {
            meshes->onMoving(transform);
        }
    }

    const AABBox<float> &Model::getDefaultModelLocalAABBox() const {
        static AABBox<float> defaultModelLocalAABBox(Point3<float>(-0.5f, -0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f));
        return defaultModelLocalAABBox;
    }

    void Model::loadAnimation(const std::string& name, const std::string& filename) {
        if (!meshes) {
            throw std::runtime_error("Cannot add animation on model without mesh");
        }

        //load and add the anim to the std::map
        auto constAnimation = ResourceRetriever::instance().getResource<ConstAnimation>(filename);
        animations[name] = std::make_unique<Animation>(constAnimation, *meshes);
        animations[name]->onMoving(transform);

        //both files must have the same number of bones
        if (meshes->getConstMeshes().getConstMesh(0).getNumberBones() != constAnimation->getNumberBones()) {
            throw std::runtime_error("Both files haven't the same number of bones. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
        }

        //we just check with mesh[0] && frame[0]
        for (unsigned int i = 0; i < meshes->getConstMeshes().getConstMesh(0).getNumberBones(); ++i) {
            //bones must have the same parent index
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).parent != constAnimation->getBone(0, i).parent) {
                throw std::runtime_error("Bones haven't the same parent index. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }

            //bones must have the same name
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).name != constAnimation->getBone(0, i).name) {
                throw std::runtime_error("Bones haven't the same name. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }
        }
    }

    bool Model::hasLoadedAnimation() const {
        return !animations.empty();
    }

    void Model::animate(const std::string& animationName, bool animationLoop) {
        activeAnimation = animations.at(animationName).get();
        isModelAnimated = true;
        stopAnimationAtLastFrame = !animationLoop;

        onMoving(transform);
    }

    void Model::stopAnimation(bool immediate) {
        if (immediate) {
            isModelAnimated = false;
        } else if (isAnimated()) {
            stopAnimationAtLastFrame = true;
        }
    }

    void Model::resetAnimations() {
        //disable current animation
        stopAnimation(true);
        if (activeAnimation) {
            activeAnimation = nullptr;
            onMoving(transform);
        }

        //reset all animations to frame 0
        for (const auto& [animName, anim] : animations) {
            if (anim->getCurrentFrame() != 0) {
                anim->gotoFrame(0);
            }
        }

        //apply skeleton bind pose
        for (unsigned int meshIndex = 0; meshIndex < meshes->getNumberMeshes(); ++meshIndex) {
            meshes->getMesh(meshIndex).resetSkeleton();
        }
        notifyMeshUpdated();
    }

    void Model::gotoAnimationFrame(const std::string& animationName, unsigned int animationFrameIndex) {
        const Animation* previousActiveAnimation = activeAnimation;
        activeAnimation = animations.at(animationName).get();
        if (previousActiveAnimation != activeAnimation) {
            onMoving(transform);
        }

        if (activeAnimation->getCurrentFrame() != animationFrameIndex) {
            activeAnimation->gotoFrame(animationFrameIndex);
            notifyMeshUpdated();
        }
    }

    const Animation* Model::getActiveAnimation() const {
        return activeAnimation;
    }

    bool Model::hasActiveAnimation() const {
        return activeAnimation != nullptr;
    }

    bool Model::isAnimated() const {
        return hasActiveAnimation() && isModelAnimated;
    }

    void Model::onMoving(const Transform<float>& newTransform) {
        //update the bounding box
        if (meshes) {
            meshes->onMoving(newTransform);
            if (hasActiveAnimation()) {
                activeAnimation->onMoving(newTransform);
            }
        } else {
            defaultModelAABBoxes[0] = Model::getDefaultModelLocalAABBox().moveAABBox(transform);
        }

        //inform the OctreeManager that the model should be updated in the octree
        this->notifyOctreeableMove();
    }

    void Model::notifyMeshUpdated() {
        this->bIsMeshUpdated = true;
        notifyObservers(this, Model::MESH_UPDATED);
    }

    const Meshes* Model::getMeshes() const {
        return meshes.get();
    }

    const ConstMeshes* Model::getConstMeshes() const {
        if (meshes) {
            return &meshes->getConstMeshes();
        }
        return nullptr;
    }

    std::map<std::string, const ConstAnimation*> Model::getAnimations() const {
        std::map<std::string, const ConstAnimation*> constConstAnimations;
        for (const auto& [animName, anim] : animations) {
            constConstAnimations.try_emplace(animName, &anim->getConstAnimation());
        }
        return constConstAnimations;
    }

    /**
    * @return Bounding box regrouping all animation frames or the bounding box regrouping all meshes (transformed by the model matrix)
    */
    const AABBox<float>& Model::getAABBox() const {
        if (hasActiveAnimation()) {
            return activeAnimation->getFramesAABBox();
        } else if (meshes) {
            return meshes->getMeshesAABBox();
        } else {
            return defaultModelAABBoxes[0];
        }
    }

    /**
    * @return Split bounding box regrouping all animation frames or the split bounding box regrouping all meshes (transformed by the model matrix).
    */
    const std::vector<AABBox<float>>& Model::getSplitAABBoxes() const {
        if (hasActiveAnimation()) {
            return activeAnimation->getFramesSplitAABBoxes();
        } else if (meshes) {
            return meshes->getMeshesSplitAABBoxes();
        } else {
            return defaultModelAABBoxes;
        }
    }

    /**
    * @return Bounding box regrouping all animation frames or the bounding box regrouping all meshes (not transformed).
    */
    const AABBox<float>& Model::getLocalAABBox() const {
        if (hasActiveAnimation()) {
            return activeAnimation->getLocalFramesAABBox();
        } else if (meshes) {
            return meshes->getLocalMeshesAABBox();
        } else {
            return Model::getDefaultModelLocalAABBox();
        }
    }

    void Model::setPosition(const Point3<float>& position) {
        if (position != transform.getPosition()) {
            transform.setPosition(position);
            onMoving(transform);
        }
    }

    void Model::setOrientation(const Quaternion<float>& orientation) {
        if (orientation != transform.getOrientation()) {
            transform.setOrientation(orientation);
            onMoving(transform);
        }
    }

    void Model::setScale(const Vector3<float>& scale) {
        if (scale != transform.getScale()) {
            transform.setScale(scale);
            onMoving(transform);
            notifyObservers(this, Model::SCALE_UPDATED);
        }
    }

    void Model::setTransform(const Transform<float>& transform) {
        if (transform != this->transform) {
            bool scaleUpdated = this->transform.getScale() != transform.getScale();
            this->transform = transform;
            onMoving(transform);
            if (scaleUpdated) {
                notifyObservers(this, Model::SCALE_UPDATED);
            }
        }
    }

    const Transform<float>& Model::getTransform() const {
        return transform;
    }

    /**
     * @param isProduceShadow Indicate whether model can produce shadow. The value should be generally set to false for ground to having better shadow rendering.
     */
    void Model::setProduceShadow(bool bIsProduceShadow) {
        this->bIsProduceShadow = bIsProduceShadow;
    }

    bool Model::isProduceShadow() const {
        return bIsProduceShadow && getMeshes();
    }

    bool Model::isMeshUpdated() const {
        return bIsMeshUpdated && getMeshes();
    }

    void Model::updateAnimation(float dt) {
        if (isAnimated()) {
            if (stopAnimationAtLastFrame && activeAnimation->getCurrentFrame() + 1 >= activeAnimation->getConstAnimation().getNumberFrames()) {
                stopAnimation(true);
                stopAnimationAtLastFrame = false;
            } else {
                activeAnimation->animate(dt);
                notifyMeshUpdated();
            }
        }
    }

    void Model::updateMesh(unsigned int meshIndex, const std::vector<Point3<float>>& vertices) {
        meshes->updateMesh(meshIndex, vertices);

        onMoving(transform);
        notifyMeshUpdated();
    }

    void Model::updateMaterial(unsigned int meshIndex, const std::shared_ptr<Material>& material) {
        meshes->updateMaterial(meshIndex, material);
        notifyObservers(this, Model::MATERIAL_UPDATED);
    }

}
