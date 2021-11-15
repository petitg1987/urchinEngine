#include <stdexcept>

#include <scene/renderer3d/model/Model.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Model::Model(const std::string& meshesFilename) :
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            bIsProduceShadow(true) {
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
            bIsProduceShadow(true) {
        initialize();
    }

    Model::Model(const Model& model) :
            Octreeable(model),
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            transform(model.getTransform()),
            bIsProduceShadow(model.isProduceShadow()) {
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
        static AABBox<float> defaultModelLocalAABBox = AABBox<float>(Point3<float>(-0.5f, -0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f));
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
            throw std::runtime_error("Both files haven't the same number of bones. Meshes filename: " + meshes->getConstMeshes().getMeshesFilename() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
        }

        //we just check with mesh[0] && frame[0]
        for (unsigned int i = 0; i < meshes->getConstMeshes().getConstMesh(0).getNumberBones(); ++i) {
            //bones must have the same parent index
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).parent != constAnimation->getBone(0, i).parent) {
                throw std::runtime_error("Bones haven't the same parent index. Meshes filename: " + meshes->getConstMeshes().getMeshesFilename() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }

            //bones must have the same name
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).name != constAnimation->getBone(0, i).name) {
                throw std::runtime_error("Bones haven't the same name. Meshes filename: " + meshes->getConstMeshes().getMeshesFilename() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }
        }
    }

    void Model::animate(const std::string& animationName, bool animationLoop) {
        activeAnimation = animations.at(animationName).get();
        isModelAnimated = true;
        stopAnimationAtLastFrame = !animationLoop;

        onMoving(transform);
    }

    void Model::stopAnimation(bool immediate) {
        if (immediate) {
            if (activeAnimation) {
                activeAnimation = nullptr;
                onMoving(transform);
            }
            isModelAnimated = false;
        } else if (isAnimated()) {
            stopAnimationAtLastFrame = true;
        }
    }

    void Model::resetAnimation(const std::string& animationName) {
        Animation* animation = animations.at(animationName).get();

        if (activeAnimation == animation) {
            stopAnimation(true);
        }

        if (animation->getCurrFrame() != 0) {
            animation->gotoFrame(0);
            notifyObservers(this, Model::MESH_UPDATED);
        }
    }

    void Model::resetSkeletonToBindPose() {
        stopAnimation(true);

        for (unsigned int meshIndex = 0; meshIndex < meshes->getNumberMeshes(); ++meshIndex) {
            meshes->getMesh(meshIndex).resetSkeleton();
        }
        notifyObservers(this, Model::MESH_UPDATED);
    }

    void Model::gotoAnimationFrame(const std::string& animationName, unsigned int animationFrameIndex) {
        activeAnimation = animations.at(animationName).get();
        onMoving(transform);

        activeAnimation->gotoFrame(animationFrameIndex);
        notifyObservers(this, Model::MESH_UPDATED);
    }

    int Model::getAnimationFrame() const {
        if (hasActiveAnimation()) {
            return (int)activeAnimation->getCurrFrame();
        }
        return -1;
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
        for (const auto& animation : animations) {
            constConstAnimations.emplace(animation.first, &animation.second->getConstAnimation());
        }
        return constConstAnimations;
    }

    /**
    * @return Bounding box regrouping all animation frames (transformed by the model matrix). If not animation exist: return meshes bounding box (transformed by the model matrix).
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
    * @return Split bounding box regrouping all animation frames (transformed by the model matrix). If not animation exist: return meshes split bounding box (transformed by the model matrix).
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
    * @return Bounding box regrouping all animation frames (not transformed). If not animation exist: return meshes bounding box (not transformed).
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

    void Model::setScale(float scale) {
        if (scale != transform.getScale()) {
            transform.setScale(scale);
            onMoving(transform);
        }
    }

    void Model::setTransform(const Transform<float>& transform) {
        if (transform != this->transform) {
            this->transform = transform;
            onMoving(transform);
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
        return bIsProduceShadow;
    }

    void Model::updateAnimation(float dt) {
        if (isAnimated()) {
            if (stopAnimationAtLastFrame && activeAnimation->getCurrFrame() + 1 >= activeAnimation->getConstAnimation().getNumberFrames()) {
                stopAnimation(true);
                stopAnimationAtLastFrame = false;
            } else {
                activeAnimation->animate(dt);
                notifyObservers(this, Model::MESH_UPDATED);
            }
        }
    }

    void Model::updateMesh(unsigned int meshIndex, const std::vector<Point3<float>>& vertices) {
        meshes->updateMesh(meshIndex, vertices);

        onMoving(transform);
        notifyObservers(this, Model::MESH_UPDATED);
    }

    void Model::updateMaterial(unsigned int meshIndex, const std::shared_ptr<Material>& material) {
        meshes->updateMaterial(meshIndex, material);
        notifyObservers(this, Model::MATERIAL_UPDATED);
    }

}
