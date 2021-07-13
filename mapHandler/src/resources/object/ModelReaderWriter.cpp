#include <vector>

#include "ModelReaderWriter.h"
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    Model* ModelReaderWriter::loadFrom(const DataChunk* modelChunk, const DataParser& dataParser) {
        auto meshChunk = dataParser.getUniqueChunk(true, MESH_TAG, UdaAttribute(), modelChunk);
        auto meshFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), meshChunk);

        auto* model = new Model(meshFilenameChunk->getStringValue());
        loadAnimationsOn(model, modelChunk, dataParser);
        loadTransformOn(model, modelChunk, dataParser);
        loadFlagsOn(model, modelChunk, dataParser);

        return model;
    }

    void ModelReaderWriter::writeOn(DataChunk& modelChunk, const Model* model, UdaWriter& udaWriter) {
        auto& meshChunk = udaWriter.createChunk(MESH_TAG, UdaAttribute(), &modelChunk);
        auto& meshFilenameChunk = udaWriter.createChunk(FILENAME_TAG, UdaAttribute(), &meshChunk);

        if (model->getConstMeshes()) {
            meshFilenameChunk.setStringValue(model->getConstMeshes()->getMeshFilename());
        }
        writeAnimationsOn(modelChunk, model, udaWriter);
        writeTransformOn(modelChunk, model, udaWriter);
        writeFlagsOn(modelChunk, model, udaWriter);
    }

    void ModelReaderWriter::loadAnimationsOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto animationsListChunk = dataParser.getUniqueChunk(false, ANIMATIONS_TAG, UdaAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = dataParser.getChunks(ANIMATION_TAG, UdaAttribute(), animationsListChunk);
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = dataParser.getUniqueChunk(true, NAME_TAG, UdaAttribute(), animationChunk);
                auto animationFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, UdaAttribute(), animationChunk);

                model->loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimationsOn(DataChunk& modelChunk, const Model* model, UdaWriter& udaWriter) {
        std::map<std::string, const ConstAnimation*> animations = model->getAnimations();
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

    void ModelReaderWriter::loadTransformOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto transformChunk = dataParser.getUniqueChunk(true, TRANSFORM_TAG, UdaAttribute(), modelChunk);

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk, dataParser);

        auto scaleChunk = dataParser.getUniqueChunk(false, SCALE_TAG, UdaAttribute(), transformChunk);
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model->setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransformOn(DataChunk& modelChunk, const Model* model, UdaWriter& udaWriter) {
        auto& transformChunk = udaWriter.createChunk(TRANSFORM_TAG, UdaAttribute(), &modelChunk);

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model->getTransform().getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk, model->getTransform().getOrientation(), udaWriter);

        auto& scaleChunk = udaWriter.createChunk(SCALE_TAG, UdaAttribute(), &transformChunk);
        scaleChunk.setFloatValue(model->getTransform().getScale());
    }

    void ModelReaderWriter::loadFlagsOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto produceShadowChunk = dataParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, UdaAttribute(), modelChunk);
        if (produceShadowChunk) {
            model->setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model->setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlagsOn(DataChunk& modelChunk, const Model* model, UdaWriter& udaWriter) {
        if (!model->isProduceShadow()) {
            auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &modelChunk);
            produceShadowChunk.setBoolValue(model->isProduceShadow());
        }
    }

}
