#include <stdexcept>

#include <scene/renderer3d/model/Model.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Model::Model(const std::string& meshesFilename) :
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            currAnimation(nullptr),
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
            currAnimation(nullptr),
            stopAnimationAtLastFrame(false),
            bIsProduceShadow(true) {
        initialize();
    }

    Model::Model(const Model& model) :
            Octreeable(model),
            defaultModelAABBoxes({Model::getDefaultModelLocalAABBox()}),
            currAnimation(nullptr),
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
            throw std::runtime_error("Both files haven't the same number of bones. Meshes filename: " + meshes->getConstMeshes().getName() + ", Animation filename: " + constAnimation->getName() + ".");
        }

        //we just check with mesh[0] && frame[0]
        for (unsigned int i = 0; i < meshes->getConstMeshes().getConstMesh(0).getNumberBones(); ++i) {
            //bones must have the same parent index
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).parent != constAnimation->getBone(0, i).parent) {
                throw std::runtime_error("Bones haven't the same parent index. Meshes filename: " + meshes->getConstMeshes().getName() + ", Animation filename: " + constAnimation->getName() + ".");
            }

            //bones must have the same name
            if (meshes->getConstMeshes().getConstMesh(0).getBaseBone(i).name != constAnimation->getBone(0, i).name) {
                throw std::runtime_error("Bones haven't the same name. Meshes filename: " + meshes->getConstMeshes().getName() + ", Animation filename: " + constAnimation->getName() + ".");
            }
        }
    }

    void Model::animate(const std::string& animationName) {
        currAnimation = animations.at(animationName).get();

        onMoving(transform);
    }

    void Model::stopAnimation(bool immediate) {
        if (immediate) {
            currAnimation = nullptr;
        } else if (isAnimate()) {
            stopAnimationAtLastFrame = true;
        }

        onMoving(transform);
    }

    bool Model::isAnimate() const {
        return currAnimation != nullptr;
    }

    void Model::onMoving(const Transform<float>& newTransform) {
        //update the bounding box
        if (meshes) {
            meshes->onMoving(newTransform);
            if (currAnimation) {
                currAnimation->onMoving(newTransform);
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
    * @return Merged bounding box for all animations. If not animation exist: return meshes bounding box.
    */
    const AABBox<float>& Model::getAABBox() const {
        if (isAnimate()) {
            return currAnimation->getGlobalAABBox();
        } else if (meshes) {
            return meshes->getGlobalAABBox();
        } else {
            return defaultModelAABBoxes[0];
        }
    }

    /**
     * @return identical to getAABBox() method but the bounding box is split to the limit size configured
     */
    const std::vector<AABBox<float>>& Model::getSplitAABBoxes() const {
        if (isAnimate()) {
            return currAnimation->getGlobalSplitAABBoxes();
        } else if (meshes) {
            return meshes->getGlobalSplitAABBoxes();
        } else {
            return defaultModelAABBoxes;
        }
    }

    /**
    * @return Local merged bounding box for all animations. If not animation exist: return local meshes bounding box.
    */
    const AABBox<float>& Model::getLocalAABBox() const {
        if (isAnimate()) {
            return currAnimation->getGlobalLocalAABBox();
        } else if (meshes) {
            return meshes->getGlobalLocalAABBox();
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
        this->transform = transform;
        onMoving(transform);
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
        //animate model
        if (isAnimate()) {
            if (stopAnimationAtLastFrame && currAnimation->getCurrFrame() == 0u) {
                stopAnimation(true);
                stopAnimationAtLastFrame = false;
            } else {
                currAnimation->animate(dt);
                notifyObservers(this, Model::MESH_UPDATED);
            }
        }

    }

}
