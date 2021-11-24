#include <resources/model/ModelReaderWriter.h>
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    std::shared_ptr<Model> ModelReaderWriter::loadFrom(const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto meshesChunk = udaParser.getUniqueChunk(true, MESHES_TAG, UdaAttribute(), modelChunk);
        auto meshesFilenameChunk = udaParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), meshesChunk);

        auto model = Model::fromMeshesFile(meshesFilenameChunk->getStringValue());
        loadAnimationsOn(*model, modelChunk, udaParser);
        loadTransformOn(*model, modelChunk, udaParser);
        loadFlagsOn(*model, modelChunk, udaParser);
        loadTagsOn(*model, modelChunk, udaParser);

        return model;
    }

    void ModelReaderWriter::writeOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& meshesChunk = udaWriter.createChunk(MESHES_TAG, UdaAttribute(), &modelChunk);
        auto& meshesFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &meshesChunk);

        if (model.getConstMeshes()) {
            meshesFilenameChunk.setStringValue(model.getConstMeshes()->getMeshesFilename());
        }
        writeAnimationsOn(modelChunk, model, udaWriter);
        writeTransformOn(modelChunk, model, udaWriter);
        writeFlagsOn(modelChunk, model, udaWriter);
        writeTagsOn(modelChunk, model, udaWriter);
    }

    void ModelReaderWriter::loadAnimationsOn(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto animationsListChunk = udaParser.getUniqueChunk(false, ANIMATIONS_TAG, UdaAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = udaParser.getChunks(ANIMATION_TAG, UdaAttribute(), animationsListChunk);
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = udaParser.getUniqueChunk(true, NAME_TAG, UdaAttribute(), animationChunk);
                auto animationFilenameChunk = udaParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), animationChunk);

                model.loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimationsOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        std::map<std::string, const ConstAnimation*> animations = model.getAnimations();
        if (!animations.empty()) {
            auto& animationsListChunk = udaWriter.createChunk(ANIMATIONS_TAG, UdaAttribute(), &modelChunk);
            for (const auto& [animName, constAnimation] : animations) {
                auto& animationChunk = udaWriter.createChunk(ANIMATION_TAG, UdaAttribute(), &animationsListChunk);

                auto& animationNameChunk = udaWriter.createChunk(NAME_TAG, UdaAttribute(), &animationChunk);
                auto& animationFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &animationChunk);

                animationNameChunk.setStringValue(animName);
                animationFilenameChunk.setStringValue(constAnimation->getAnimationFilename());
            }
        }
    }

    void ModelReaderWriter::loadTransformOn(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto transformChunk = udaParser.getUniqueChunk(true, TRANSFORM_TAG, UdaAttribute(), modelChunk);

        auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk, udaParser);

        auto scaleChunk = udaParser.getUniqueChunk(false, SCALE_TAG, UdaAttribute(), transformChunk);
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model.setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransformOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& transformChunk = udaWriter.createChunk(TRANSFORM_TAG, UdaAttribute(), &modelChunk);

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model.getTransform().getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk, model.getTransform().getOrientation(), udaWriter);

        auto& scaleChunk = udaWriter.createChunk(SCALE_TAG, UdaAttribute(), &transformChunk);
        scaleChunk.setFloatValue(model.getTransform().getScale());
    }

    void ModelReaderWriter::loadFlagsOn(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto produceShadowChunk = udaParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, UdaAttribute(), modelChunk);
        if (produceShadowChunk) {
            model.setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model.setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlagsOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        if (!model.isProduceShadow()) {
            auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &modelChunk);
            produceShadowChunk.setBoolValue(model.isProduceShadow());
        }
    }

    void ModelReaderWriter::loadTagsOn(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto tagsChunk = udaParser.getUniqueChunk(false, TAGS_TAG, UdaAttribute(), modelChunk);
        if (tagsChunk) {
            std::string tagsValues = tagsChunk->getStringValue();
            std::vector<std::string> tags = StringUtil::split(tagsValues, TAGS_SEPARATOR);
            for (const std::string& tag: tags) {
                model.addTag(tag);
            }
        }
    }

    void ModelReaderWriter::writeTagsOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        std::string tagsValues = StringUtil::merge(model.getTags(), TAGS_SEPARATOR);
        if (!tagsValues.empty()) {
            auto& tagsChunk = udaWriter.createChunk(TAGS_TAG, UdaAttribute(), &modelChunk);
            tagsChunk.setStringValue(tagsValues);
        }
    }

}
