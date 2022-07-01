#include <stdexcept>
#include <algorithm>

#include <scene/renderer3d/model/Model.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Model::Model(const std::string& meshesFilename) :
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            shadowBehavior(ShadowBehavior::RECEIVER_AND_CASTER),
            cullBehavior(CullBehavior::CULL),
            originalMeshesUpdated(false) {
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
            shadowBehavior(ShadowBehavior::RECEIVER_AND_CASTER),
            cullBehavior(CullBehavior::CULL),
            originalMeshesUpdated(false) {
        initialize();
    }

    Model::Model(const Model& model) :
            Octreeable(model),
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            activeAnimation(nullptr),
            isModelAnimated(false),
            stopAnimationAtLastFrame(false),
            transform(model.getTransform()),
            shadowBehavior(model.getShadowBehavior()),
            cullBehavior(model.getCullBehavior()),
            originalMeshesUpdated(model.isOriginalMeshesUpdated()) {
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
            meshesUpdated.resize(meshes->getNumberMeshes(), false);
            meshes->onMoving(transform);
        }
    }

    const AABBox<float> &Model::getDefaultModelLocalAABBox() const {
        static AABBox defaultModelLocalAABBox(Point3<float>(-0.5f, -0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f));
        return defaultModelLocalAABBox;
    }

    void Model::loadAnimation(const std::string& name, const std::string& filename) {
        if (!meshes) {
            throw std::runtime_error("Cannot add animation on model without mesh");
        } else if (animations.contains(name)) {
            throw std::runtime_error("Animation with name " + name + " already exist on model: " + meshes->getConstMeshes().getMeshesName());
        }

        //load and add the anim to the std::map
        auto constAnimation = ResourceRetriever::instance().getResource<ConstAnimation>(filename);
        animations.try_emplace(name, std::make_unique<Animation>(constAnimation, *meshes));
        animations[name]->onMoving(transform);

        //both files must have the same number of bones
        if (meshes->getConstMeshes().getConstMesh(0).getNumberBones() != constAnimation->getNumberBones()) {
            throw std::runtime_error("Both files have not the same number of bones. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
        }

        //check with mesh[0] && frame[0]
        for (unsigned int i = 0; i < meshes->getConstMeshes().getConstMesh(0).getNumberBones(); ++i) {
            //bones must have the same parent index
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).parent != constAnimation->getBone(0, i).parent) {
                throw std::runtime_error("Bones have not the same parent index. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }

            //bones must have the same name
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).name != constAnimation->getBone(0, i).name) {
                throw std::runtime_error("Bones have not the same name. Meshes filename: " + meshes->getConstMeshes().getMeshesName() + ", Animation filename: " + constAnimation->getAnimationFilename() + ".");
            }
        }
    }

    bool Model::hasLoadedAnimation() const {
        return !animations.empty();
    }

    void Model::animate(std::string_view animationName, AnimRepeat animLoop, AnimStart animStart) {
        auto itAnimation = animations.find(animationName);
        if (itAnimation == animations.end()) {
            throw std::invalid_argument("Unable to find " + std::string(animationName) + " animation on model: " + (getConstMeshes() ? getConstMeshes()->getMeshesName() : "[null]"));
        }
        activeAnimation = itAnimation->second.get();
        if (animStart == AnimStart::AT_FIRST_FRAME) {
            activeAnimation->gotoFrame(0);
        }
        isModelAnimated = true;
        stopAnimationAtLastFrame = (animLoop == AnimRepeat::ONCE);

        onMoving(transform);
        notifyObservers(this, Model::ANIMATION_STARTED);
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
            anim->gotoFrame(0);
        }

        //apply skeleton bind pose
        for (unsigned int meshIndex = 0; meshIndex < meshes->getNumberMeshes(); ++meshIndex) {
            meshes->getMesh(meshIndex).resetSkeleton();
        }
        notifyMeshVerticesUpdated();
    }

    void Model::gotoAnimationFrame(std::string_view animationName, unsigned int animationFrameIndex) {
        const Animation* previousActiveAnimation = activeAnimation;
        auto itAnimation = animations.find(animationName);
        if (itAnimation == animations.end()) {
            throw std::invalid_argument("Unable to find " + std::string(animationName) + " animation on model: " + (getConstMeshes() ? getConstMeshes()->getMeshesName() : "[null]"));
        }
        activeAnimation = itAnimation->second.get();
        if (previousActiveAnimation != activeAnimation) {
            onMoving(transform);
        }

        bool frameUpdated = activeAnimation->gotoFrame(animationFrameIndex);
        if (frameUpdated) {
            notifyMeshVerticesUpdatedByAnimation();
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

        if (getCullBehavior() == CullBehavior::CULL) {
            //inform the OctreeManager that the model should be updated in the octree
            this->notifyOctreeableMove();
        }
    }

    void Model::notifyMeshVerticesUpdatedByAnimation() {
        std::fill(meshesUpdated.begin(), meshesUpdated.end(), false);
        for (std::size_t updatedMeshIndex : activeAnimation->getAnimatedMeshIndices()) {
            meshesUpdated[updatedMeshIndex] = true;
        }
        originalMeshesUpdated = true;
        notifyObservers(this, Model::MESH_VERTICES_UPDATED);
    }

    void Model::notifyMeshVerticesUpdated() {
        std::fill(meshesUpdated.begin(), meshesUpdated.end(), true);

        originalMeshesUpdated = true;
        notifyObservers(this, Model::MESH_VERTICES_UPDATED);
    }

    void Model::notifyMeshVerticesUpdated(unsigned int updatedMeshIndex) {
        std::fill(meshesUpdated.begin(), meshesUpdated.end(), false);
        meshesUpdated[updatedMeshIndex] = true;

        originalMeshesUpdated = true;
        notifyObservers(this, Model::MESH_VERTICES_UPDATED);
    }

    void Model::notifyMeshUvUpdated(unsigned int updatedMeshIndex) {
        std::fill(meshesUpdated.begin(), meshesUpdated.end(), false);
        meshesUpdated[updatedMeshIndex] = true;

        originalMeshesUpdated = true;
        notifyObservers(this, Model::MESH_UV_UPDATED);
    }

    void Model::notifyMeshMaterialUpdated(unsigned int updatedMeshIndex) {
        std::fill(meshesUpdated.begin(), meshesUpdated.end(), false);
        meshesUpdated[updatedMeshIndex] = true;

        originalMeshesUpdated = true;
        notifyObservers(this, Model::MATERIAL_UPDATED);
    }

    std::string Model::getName() const {
        if (meshes) {
            return meshes->getConstMeshes().getName();
        }
        return "[no meshes]";
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

    const std::map<std::string, std::unique_ptr<Animation>, std::less<>>& Model::getAnimations() const {
        return animations;
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
                notifyObservers(this, NotificationType::SCALE_UPDATED);
            }
        }
    }

    const Transform<float>& Model::getTransform() const {
        return transform;
    }

    void Model::setShadowBehavior(ShadowBehavior shadowBehavior) {
        this->shadowBehavior = shadowBehavior;
    }

    Model::ShadowBehavior Model::getShadowBehavior() const {
        if (!getMeshes()) {
            return ShadowBehavior::NONE;
        }
        return shadowBehavior;
    }

    void Model::setCullBehavior(CullBehavior cullBehavior) {
        this->cullBehavior = cullBehavior;
    }

    Model::CullBehavior Model::getCullBehavior() const {
        return cullBehavior;
    }

    bool Model::isOriginalMeshesUpdated() const {
        return originalMeshesUpdated;
    }

    bool Model::isMeshUpdated(unsigned int meshIndex) const {
        return meshesUpdated[meshIndex];
    }

    void Model::updateAnimation(float dt) {
        if (isAnimated()) {
            if (stopAnimationAtLastFrame && activeAnimation->getCurrentFrame() + 1 >= activeAnimation->getConstAnimation().getNumberFrames()) {
                stopAnimation(true);
                stopAnimationAtLastFrame = false;
                notifyObservers(this, Model::ANIMATION_ENDED);
            } else {
                activeAnimation->animate(dt);
                notifyMeshVerticesUpdatedByAnimation();
            }
        }
    }

    void Model::updateVertices(unsigned int meshIndex, const std::vector<Point3<float>>& vertices) {
        meshes->updateMesh(meshIndex, vertices);

        onMoving(transform);
        notifyMeshVerticesUpdated(meshIndex);
    }

    void Model::updateUv(unsigned int meshIndex, const std::vector<Point2<float>>& uv) {
        meshes->updateUv(meshIndex, uv);

        notifyMeshUvUpdated(meshIndex);
    }

    void Model::updateMaterial(unsigned int meshIndex, std::shared_ptr<Material> material) {
        if (material.get() != &meshes->getMesh(meshIndex).getMaterial()) {
            meshes->updateMaterial(meshIndex, std::move(material));
            notifyMeshMaterialUpdated(meshIndex);
        }
    }

}
