#include "map/save/object/ModelReaderWriter.h"
#include "map/save/common/OrientationReaderWriter.h"
#include "util/PathUtil.h"

namespace urchin {

    std::shared_ptr<Model> ModelReaderWriter::load(const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto meshesChunk = udaParser.getFirstChunk(true, MESHES_TAG, UdaAttribute(), modelChunk);
        auto meshesFilenameChunk = udaParser.getFirstChunk(true, FILENAME_TAG, UdaAttribute(), meshesChunk);

        auto model = Model::fromMeshesFile(meshesFilenameChunk->getStringValue());
        loadAnimations(*model, modelChunk, udaParser);
        loadTransform(*model, modelChunk, udaParser);
        loadProperties(*model, modelChunk, udaParser);

        return model;
    }

    void ModelReaderWriter::write(UdaChunk& modelChunk, const Model& model, UdaParser& udaParser) {
        auto& meshesChunk = udaParser.createChunk(MESHES_TAG, UdaAttribute(), &modelChunk);
        auto& meshesFilenameChunk = udaParser.createChunk(FILENAME_TAG, UdaAttribute(), &meshesChunk);

        if (model.getConstMeshes()) {
            std::string relativeFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), model.getConstMeshes()->getMeshesFilename());
            meshesFilenameChunk.setStringValue(relativeFilename);
        }
        writeAnimations(modelChunk, model, udaParser);
        writeTransform(modelChunk, model, udaParser);
        writeProperties(modelChunk, model, udaParser);
    }

    void ModelReaderWriter::loadAnimations(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto animationsListChunk = udaParser.getFirstChunk(false, ANIMATIONS_TAG, UdaAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = udaParser.getChunks(ANIMATION_TAG, UdaAttribute(), animationsListChunk);
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = udaParser.getFirstChunk(true, NAME_TAG, UdaAttribute(), animationChunk);
                auto animationFilenameChunk = udaParser.getFirstChunk(true, FILENAME_TAG, UdaAttribute(), animationChunk);

                model.loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimations(UdaChunk& modelChunk, const Model& model, UdaParser& udaParser) {
        const std::map<std::string, std::unique_ptr<Animation>, std::less<>>& animations = model.getAnimations();
        if (!animations.empty()) {
            auto& animationsListChunk = udaParser.createChunk(ANIMATIONS_TAG, UdaAttribute(), &modelChunk);
            for (const auto& [animName, anim] : animations) {
                auto& animationChunk = udaParser.createChunk(ANIMATION_TAG, UdaAttribute(), &animationsListChunk);

                auto& animationNameChunk = udaParser.createChunk(NAME_TAG, UdaAttribute(), &animationChunk);
                auto& animationFilenameChunk = udaParser.createChunk(FILENAME_TAG, UdaAttribute(), &animationChunk);

                animationNameChunk.setStringValue(animName);
                animationFilenameChunk.setStringValue(anim->getConstAnimation().getAnimationFilename());
            }
        }
    }

    void ModelReaderWriter::loadTransform(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto transformChunk = udaParser.getFirstChunk(true, TRANSFORM_TAG, UdaAttribute(), modelChunk);

        auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::load(transformChunk, udaParser);

        auto scaleChunk = udaParser.getFirstChunk(false, SCALE_TAG, UdaAttribute(), transformChunk);
        Vector3 scale(1.0f, 1.0f, 1.0f);
        if (scaleChunk) {
            scale = scaleChunk->getVector3Value();
        }

        model.setTransform(Transform(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransform(UdaChunk& modelChunk, const Model& model, UdaParser& udaParser) {
        auto& transformChunk = udaParser.createChunk(TRANSFORM_TAG, UdaAttribute(), &modelChunk);

        auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model.getTransform().getPosition());

        OrientationReaderWriter::write(transformChunk, model.getTransform().getOrientation(), udaParser);

        auto& scaleChunk = udaParser.createChunk(SCALE_TAG, UdaAttribute(), &transformChunk);
        scaleChunk.setVector3Value(model.getTransform().getScale());
    }

    void ModelReaderWriter::loadProperties(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto shadowBehaviorChunk = udaParser.getFirstChunk(false, SHADOW_BEHAVIOR_TAG, UdaAttribute(), modelChunk);
        if (shadowBehaviorChunk) {
            if (shadowBehaviorChunk->getStringValue() == RECEIVER_AND_CASTER_VALUE) {
                model.setShadowBehavior(Model::ShadowBehavior::RECEIVER_AND_CASTER);
            } else if (shadowBehaviorChunk->getStringValue() == RECEIVER_ONLY_VALUE) {
                model.setShadowBehavior(Model::ShadowBehavior::RECEIVER_ONLY);
            } else if (shadowBehaviorChunk->getStringValue() == NONE_VALUE) {
                model.setShadowBehavior(Model::ShadowBehavior::NONE);
            } else {
                throw std::runtime_error("Unknown shadow behavior value: " + shadowBehaviorChunk->getStringValue());
            }
        } else {
            model.setShadowBehavior(Model::ShadowBehavior::RECEIVER_AND_CASTER);
        }

        auto cullBehaviorChunk = udaParser.getFirstChunk(false, CULL_BEHAVIOR_TAG, UdaAttribute(), modelChunk);
        if (cullBehaviorChunk) {
            if (cullBehaviorChunk->getStringValue() == CULL_VALUE) {
                model.setCullBehavior(Model::CullBehavior::CULL);
            } else if (cullBehaviorChunk->getStringValue() == NO_CULL_VALUE) {
                model.setCullBehavior(Model::CullBehavior::NO_CULL);
            } else {
                throw std::runtime_error("Unknown cull behavior value: " + cullBehaviorChunk->getStringValue());
            }
        } else {
            model.setCullBehavior(Model::CullBehavior::CULL);
        }
    }

    void ModelReaderWriter::writeProperties(UdaChunk& modelChunk, const Model& model, UdaParser& udaParser) {
        if (model.getShadowBehavior() != Model::ShadowBehavior::RECEIVER_AND_CASTER) { //not default value
            auto& shadowClassChunk = udaParser.createChunk(SHADOW_BEHAVIOR_TAG, UdaAttribute(), &modelChunk);
            if (model.getShadowBehavior() == Model::ShadowBehavior::RECEIVER_ONLY) {
                shadowClassChunk.setStringValue(RECEIVER_ONLY_VALUE);
            } else if (model.getShadowBehavior() == Model::ShadowBehavior::NONE) {
                shadowClassChunk.setStringValue(NONE_VALUE);
            } else {
                throw std::runtime_error("Unknown shadow class: " + std::to_string((int)model.getShadowBehavior()));
            }
        }

        if (model.getCullBehavior() != Model::CullBehavior::CULL) { //not default value
            auto& cullBehaviorChunk = udaParser.createChunk(CULL_BEHAVIOR_TAG, UdaAttribute(), &modelChunk);
            if (model.getCullBehavior() == Model::CullBehavior::NO_CULL) {
                cullBehaviorChunk.setStringValue(NO_CULL_VALUE);
            } else {
                throw std::runtime_error("Unknown cull behavior: " + std::to_string((int)model.getCullBehavior()));
            }
        }
    }

}
