#include <map/save/object/ModelReaderWriter.h>
#include <map/save/common/OrientationReaderWriter.h>

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

    void ModelReaderWriter::write(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& meshesChunk = udaWriter.createChunk(MESHES_TAG, UdaAttribute(), &modelChunk);
        auto& meshesFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &meshesChunk);

        if (model.getConstMeshes()) {
            std::string relativeFilename = FileUtil::getRelativePath(FileSystem::instance().getResourcesDirectory(), model.getConstMeshes()->getMeshesFilename());
            meshesFilenameChunk.setStringValue(relativeFilename);
        }
        writeAnimations(modelChunk, model, udaWriter);
        writeTransform(modelChunk, model, udaWriter);
        writeProperties(modelChunk, model, udaWriter);
    }

    void ModelReaderWriter::loadAnimations(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto animationsListChunk = udaParser.getFirstChunk(false, ANIMATIONS_TAG, UdaAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = udaParser.getChunks(ANIMATION_TAG, UdaAttribute(), animationsListChunk);
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = udaParser.getFirstChunk(true, NAME_TAG, UdaAttribute(), animationChunk);
                auto animationFilenameChunk = udaParser.getFirstChunk(true, FILENAME_TAG, UdaAttribute(), animationChunk);
                auto animationShadowImpactChunk = udaParser.getFirstChunk(true, ANIM_SHADOW_IMPACT, UdaAttribute(), animationChunk);

                model.loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue(), static_cast<AnimShadowImpact>(animationShadowImpactChunk->getIntValue()));
            }
        }
    }

    void ModelReaderWriter::writeAnimations(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        const std::map<std::string, std::unique_ptr<Animation>, std::less<>>& animations = model.getAnimations();
        if (!animations.empty()) {
            auto& animationsListChunk = udaWriter.createChunk(ANIMATIONS_TAG, UdaAttribute(), &modelChunk);
            for (const auto& [animName, anim] : animations) {
                auto& animationChunk = udaWriter.createChunk(ANIMATION_TAG, UdaAttribute(), &animationsListChunk);

                auto& animationNameChunk = udaWriter.createChunk(NAME_TAG, UdaAttribute(), &animationChunk);
                auto& animationFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &animationChunk);
                auto& animationShadowImpactChunk = udaWriter.createChunk(ANIM_SHADOW_IMPACT, UdaAttribute(), &animationChunk);

                animationNameChunk.setStringValue(animName);
                animationFilenameChunk.setStringValue(anim->getConstAnimation().getAnimationFilename());
                animationShadowImpactChunk.setIntValue((int)anim->getShadowImpact());
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

        model.setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransform(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& transformChunk = udaWriter.createChunk(TRANSFORM_TAG, UdaAttribute(), &modelChunk);

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model.getTransform().getPosition());

        OrientationReaderWriter::write(transformChunk, model.getTransform().getOrientation(), udaWriter);

        auto& scaleChunk = udaWriter.createChunk(SCALE_TAG, UdaAttribute(), &transformChunk);
        scaleChunk.setVector3Value(model.getTransform().getScale());
    }

    void ModelReaderWriter::loadProperties(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto shadowClassChunk = udaParser.getFirstChunk(false, SHADOW_CLASS_TAG, UdaAttribute(), modelChunk);
        if (shadowClassChunk) {
            if (shadowClassChunk->getStringValue() == RECEIVER_AND_CASTER_VALUE) {
                model.setShadowClass(Model::RECEIVER_AND_CASTER);
            } else if (shadowClassChunk->getStringValue() == RECEIVER_ONLY_VALUE) {
                model.setShadowClass(Model::RECEIVER_ONLY);
            } else if (shadowClassChunk->getStringValue() == NONE_VALUE) {
                model.setShadowClass(Model::NONE);
            } else {
                throw std::runtime_error("Unknown shadow class value: " + shadowClassChunk->getStringValue());
            }
        } else {
            model.setShadowClass(Model::RECEIVER_AND_CASTER);
        }
    }

    void ModelReaderWriter::writeProperties(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& shadowClassChunk = udaWriter.createChunk(SHADOW_CLASS_TAG, UdaAttribute(), &modelChunk);

        if (model.getShadowClass() == Model::RECEIVER_AND_CASTER) {
            shadowClassChunk.setStringValue(RECEIVER_AND_CASTER_VALUE);
        } else if (model.getShadowClass() == Model::RECEIVER_ONLY) {
            shadowClassChunk.setStringValue(RECEIVER_ONLY_VALUE);
        } else if (model.getShadowClass() == Model::NONE) {
            shadowClassChunk.setStringValue(NONE_VALUE);
        } else {
            throw std::runtime_error("Unknown shadow class: " + std::to_string(model.getShadowClass()));
        }
    }

}
