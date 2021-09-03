#include <resources/object/ModelReaderWriter.h>
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    std::shared_ptr<Model> ModelReaderWriter::loadFrom(const UdaChunk* modelChunk, const UdaParser& udaParser) {
        auto meshChunk = udaParser.getUniqueChunk(true, MESHES_TAG, UdaAttribute(), modelChunk);
        auto meshesFilenameChunk = udaParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), meshChunk);

        auto model = std::make_shared<Model>(meshesFilenameChunk->getStringValue());
        loadAnimationsOn(*model, modelChunk, udaParser);
        loadTransformOn(*model, modelChunk, udaParser);
        loadFlagsOn(*model, modelChunk, udaParser);

        return model;
    }

    void ModelReaderWriter::writeOn(UdaChunk& modelChunk, const Model& model, UdaWriter& udaWriter) {
        auto& meshChunk = udaWriter.createChunk(MESHES_TAG, UdaAttribute(), &modelChunk);
        auto& meshFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &meshChunk);

        if (model.getConstMeshes()) {
            meshFilenameChunk.setStringValue(model.getConstMeshes()->getMeshFilename());
        }
        writeAnimationsOn(modelChunk, model, udaWriter);
        writeTransformOn(modelChunk, model, udaWriter);
        writeFlagsOn(modelChunk, model, udaWriter);
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
            for (auto& animation : animations) {
                auto& animationChunk = udaWriter.createChunk(ANIMATION_TAG, UdaAttribute(), &animationsListChunk);

                auto& animationNameChunk = udaWriter.createChunk(NAME_TAG, UdaAttribute(), &animationChunk);
                auto& animationFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &animationChunk);

                animationNameChunk.setStringValue(animation.first);
                animationFilenameChunk.setStringValue(animation.second->getAnimationFilename());
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

}
