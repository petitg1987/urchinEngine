#include <vector>

#include "ModelReaderWriter.h"
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    Model* ModelReaderWriter::loadFrom(const XmlChunk* modelChunk, const DataParser& dataParser) {
        auto meshChunk = dataParser.getUniqueChunk(true, MESH_TAG, DataAttribute(), modelChunk);
        auto meshFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, DataAttribute(), meshChunk.get());

        auto* model = new Model(meshFilenameChunk->getStringValue());
        loadAnimationsOn(model, modelChunk, dataParser);
        loadTransformOn(model, modelChunk, dataParser);
        loadFlagsOn(model, modelChunk, dataParser);

        return model;
    }

    void ModelReaderWriter::writeOn(XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        auto meshChunk = xmlWriter.createChunk(MESH_TAG, DataAttribute(), modelChunk);
        auto meshFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, DataAttribute(), meshChunk.get());

        if (model->getConstMeshes()) {
            meshFilenameChunk->setStringValue(model->getConstMeshes()->getMeshFilename());
        }
        writeAnimationsOn(modelChunk, model, xmlWriter);
        writeTransformOn(modelChunk, model, xmlWriter);
        writeFlagsOn(modelChunk, model, xmlWriter);
    }

    void ModelReaderWriter::loadAnimationsOn(Model* model, const XmlChunk* modelChunk, const DataParser& dataParser) {
        auto animationsListChunk = dataParser.getUniqueChunk(false, ANIMATIONS_TAG, DataAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = dataParser.getChunks(ANIMATION_TAG, DataAttribute(), animationsListChunk.get());
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = dataParser.getUniqueChunk(true, NAME_TAG, DataAttribute(), animationChunk.get());
                auto animationFilenameChunk = dataParser.getUniqueChunk(true, FILENAME_TAG, DataAttribute(), animationChunk.get());

                model->loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimationsOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        std::map<std::string, const ConstAnimation*> animations = model->getAnimations();
        if (!animations.empty()) {
            auto animationsListChunk = xmlWriter.createChunk(ANIMATIONS_TAG, DataAttribute(), modelChunk);
            for (auto& animation : animations) {
                auto animationChunk = xmlWriter.createChunk(ANIMATION_TAG, DataAttribute(), animationsListChunk.get());

                auto animationNameChunk = xmlWriter.createChunk(NAME_TAG, DataAttribute(), animationChunk.get());
                auto animationFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, DataAttribute(), animationChunk.get());

                animationNameChunk->setStringValue(animation.first);
                animationFilenameChunk->setStringValue(animation.second->getAnimationFilename());
            }
        }
    }

    void ModelReaderWriter::loadTransformOn(Model* model, const XmlChunk* modelChunk, const DataParser& dataParser) {
        auto transformChunk = dataParser.getUniqueChunk(true, TRANSFORM_TAG, DataAttribute(), modelChunk);

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), transformChunk.get());
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk.get(), dataParser);

        auto scaleChunk = dataParser.getUniqueChunk(false, SCALE_TAG, DataAttribute(), transformChunk.get());
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model->setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransformOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        auto transformChunk = xmlWriter.createChunk(TRANSFORM_TAG, DataAttribute(), modelChunk);

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, DataAttribute(), transformChunk.get());
        positionChunk->setPoint3Value(model->getTransform().getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk.get(), model->getTransform().getOrientation(), xmlWriter);

        auto scaleChunk = xmlWriter.createChunk(SCALE_TAG, DataAttribute(), transformChunk.get());
        scaleChunk->setFloatValue(model->getTransform().getScale());
    }

    void ModelReaderWriter::loadFlagsOn(Model* model, const XmlChunk* modelChunk, const DataParser& dataParser) {
        auto produceShadowChunk = dataParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, DataAttribute(), modelChunk);
        if (produceShadowChunk) {
            model->setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model->setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlagsOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        if (!model->isProduceShadow()) {
            auto produceShadowChunk = xmlWriter.createChunk(PRODUCE_SHADOW_TAG, DataAttribute(), modelChunk);
            produceShadowChunk->setBoolValue(model->isProduceShadow());
        }
    }

}
