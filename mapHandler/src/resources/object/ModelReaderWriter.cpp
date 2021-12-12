#include <resources/object/ModelReaderWriter.h>
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    std::shared_ptr<Model> ModelReaderWriter::load(const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto meshesChunk = udaParser.getUniqueChunk(true, MESHES_TAG, UdaAttribute(), modelChunk);
        auto meshesFilenameChunk = udaParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), meshesChunk);

        auto model = Model::fromMeshesFile(meshesFilenameChunk->getStringValue());
        loadAnimations(*model, modelChunk, udaParser);
        loadTransform(*model, modelChunk, udaParser);
        loadFlags(*model, modelChunk, udaParser);

        return model;
    }

    void ModelReaderWriter::write(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& meshesChunk = udaWriter.createChunk(MESHES_TAG, UdaAttribute(), &modelChunk);
        auto& meshesFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &meshesChunk);

        if (model.getConstMeshes()) {
            meshesFilenameChunk.setStringValue(model.getConstMeshes()->getMeshesFilename());
        }
        writeAnimations(modelChunk, model, udaWriter);
        writeTransform(modelChunk, model, udaWriter);
        writeFlags(modelChunk, model, udaWriter);
    }

    void ModelReaderWriter::loadAnimations(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
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

    void ModelReaderWriter::writeAnimations(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
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

    void ModelReaderWriter::loadTransform(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto transformChunk = udaParser.getUniqueChunk(true, TRANSFORM_TAG, UdaAttribute(), modelChunk);

        auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::load(transformChunk, udaParser);

        auto scaleChunk = udaParser.getUniqueChunk(false, SCALE_TAG, UdaAttribute(), transformChunk);
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model.setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransform(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& transformChunk = udaWriter.createChunk(TRANSFORM_TAG, UdaAttribute(), &modelChunk);

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model.getTransform().getPosition());

        OrientationReaderWriter::write(transformChunk, model.getTransform().getOrientation(), udaWriter);

        auto& scaleChunk = udaWriter.createChunk(SCALE_TAG, UdaAttribute(), &transformChunk);
        scaleChunk.setFloatValue(model.getTransform().getScale());
    }

    void ModelReaderWriter::loadFlags(Model& model, const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto produceShadowChunk = udaParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, UdaAttribute(), modelChunk);
        if (produceShadowChunk) {
            model.setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model.setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlags(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        if (!model.isProduceShadow()) {
            auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &modelChunk);
            produceShadowChunk.setBoolValue(model.isProduceShadow());
        }
    }

}
