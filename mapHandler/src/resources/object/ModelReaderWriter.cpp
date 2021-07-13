#include <vector>

#include "ModelReaderWriter.h"
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    Model* ModelReaderWriter::loadFrom(const DataChunk* modelChunk, const DataParser& dataParser) {
        auto meshChunk = dataParser.getUniqueChunk(true, MESH_TAG, DataAttribute(), modelChunk);
        auto meshFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, DataAttribute(), meshChunk);

        auto* model = new Model(meshFilenameChunk->getStringValue());
        loadAnimationsOn(model, modelChunk, dataParser);
        loadTransformOn(model, modelChunk, dataParser);
        loadFlagsOn(model, modelChunk, dataParser);

        return model;
    }

    void ModelReaderWriter::writeOn(DataChunk& modelChunk, const Model* model, DataWriter& dataWriter) {
        auto& meshChunk = dataWriter.createChunk(MESH_TAG, DataAttribute(), &modelChunk);
        auto& meshFilenameChunk = dataWriter.createChunk(FILENAME_TAG, DataAttribute(), &meshChunk);

        if (model->getConstMeshes()) {
            meshFilenameChunk.setStringValue(model->getConstMeshes()->getMeshFilename());
        }
        writeAnimationsOn(modelChunk, model, dataWriter);
        writeTransformOn(modelChunk, model, dataWriter);
        writeFlagsOn(modelChunk, model, dataWriter);
    }

    void ModelReaderWriter::loadAnimationsOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto animationsListChunk = dataParser.getUniqueChunk(false, ANIMATIONS_TAG, DataAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = dataParser.getChunks(ANIMATION_TAG, DataAttribute(), animationsListChunk);
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = dataParser.getUniqueChunk(true, NAME_TAG, DataAttribute(), animationChunk);
                auto animationFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, DataAttribute(), animationChunk);

                model->loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimationsOn(DataChunk& modelChunk, const Model* model, DataWriter& dataWriter) {
        std::map<std::string, const ConstAnimation*> animations = model->getAnimations();
        if (!animations.empty()) {
            auto& animationsListChunk = dataWriter.createChunk(ANIMATIONS_TAG, DataAttribute(), &modelChunk);
            for (auto& animation : animations) {
                auto& animationChunk = dataWriter.createChunk(ANIMATION_TAG, DataAttribute(), &animationsListChunk);

                auto& animationNameChunk = dataWriter.createChunk(NAME_TAG, DataAttribute(), &animationChunk);
                auto& animationFilenameChunk = dataWriter.createChunk(FILENAME_TAG, DataAttribute(), &animationChunk);

                animationNameChunk.setStringValue(animation.first);
                animationFilenameChunk.setStringValue(animation.second->getAnimationFilename());
            }
        }
    }

    void ModelReaderWriter::loadTransformOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto transformChunk = dataParser.getUniqueChunk(true, TRANSFORM_TAG, DataAttribute(), modelChunk);

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), transformChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk, dataParser);

        auto scaleChunk = dataParser.getUniqueChunk(false, SCALE_TAG, DataAttribute(), transformChunk);
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model->setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransformOn(DataChunk& modelChunk, const Model* model, DataWriter& dataWriter) {
        auto& transformChunk = dataWriter.createChunk(TRANSFORM_TAG, DataAttribute(), &modelChunk);

        auto& positionChunk = dataWriter.createChunk(POSITION_TAG, DataAttribute(), &transformChunk);
        positionChunk.setPoint3Value(model->getTransform().getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk, model->getTransform().getOrientation(), dataWriter);

        auto& scaleChunk = dataWriter.createChunk(SCALE_TAG, DataAttribute(), &transformChunk);
        scaleChunk.setFloatValue(model->getTransform().getScale());
    }

    void ModelReaderWriter::loadFlagsOn(Model* model, const DataChunk* modelChunk, const DataParser& dataParser) {
        auto produceShadowChunk = dataParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, DataAttribute(), modelChunk);
        if (produceShadowChunk) {
            model->setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model->setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlagsOn(DataChunk& modelChunk, const Model* model, DataWriter& dataWriter) {
        if (!model->isProduceShadow()) {
            auto& produceShadowChunk = dataWriter.createChunk(PRODUCE_SHADOW_TAG, DataAttribute(), &modelChunk);
            produceShadowChunk.setBoolValue(model->isProduceShadow());
        }
    }

}
