#include <vector>

#include "ModelReaderWriter.h"
#include <resources/common/OrientationReaderWriter.h>

namespace urchin {

    Model* ModelReaderWriter::loadFrom(const XmlChunk* modelChunk, const XmlParser& xmlParser) {
        auto meshChunk = xmlParser.getUniqueChunk(true, MESH_TAG, XmlAttribute(), modelChunk);
        auto meshFilenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), meshChunk.get());

        auto* model = new Model(meshFilenameChunk->getStringValue());
        loadAnimationsOn(model, modelChunk, xmlParser);
        loadTransformOn(model, modelChunk, xmlParser);
        loadFlagsOn(model, modelChunk, xmlParser);

        return model;
    }

    void ModelReaderWriter::writeOn(XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        auto meshChunk = xmlWriter.createChunk(MESH_TAG, XmlAttribute(), modelChunk);
        auto meshFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), meshChunk.get());

        if (model->getConstMeshes()) {
            meshFilenameChunk->setStringValue(model->getConstMeshes()->getMeshFilename());
        }
        writeAnimationsOn(modelChunk, model, xmlWriter);
        writeTransformOn(modelChunk, model, xmlWriter);
        writeFlagsOn(modelChunk, model, xmlWriter);
    }

    void ModelReaderWriter::loadAnimationsOn(Model* model, const XmlChunk* modelChunk, const XmlParser& xmlParser) {
        auto animationsListChunk = xmlParser.getUniqueChunk(false, ANIMATIONS_TAG, XmlAttribute(), modelChunk);
        if (animationsListChunk) {
            auto animationsChunk = xmlParser.getChunks(ANIMATION_TAG, XmlAttribute(), animationsListChunk.get());
            for (const auto& animationChunk : animationsChunk) {
                auto animationNameChunk = xmlParser.getUniqueChunk(true, NAME_TAG, XmlAttribute(), animationChunk.get());
                auto animationFilenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), animationChunk.get());

                model->loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
            }
        }
    }

    void ModelReaderWriter::writeAnimationsOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        std::map<std::string, const ConstAnimation*> animations = model->getAnimations();
        if (!animations.empty()) {
            auto animationsListChunk = xmlWriter.createChunk(ANIMATIONS_TAG, XmlAttribute(), modelChunk);
            for (auto& animation : animations) {
                auto animationChunk = xmlWriter.createChunk(ANIMATION_TAG, XmlAttribute(), animationsListChunk.get());

                auto animationNameChunk = xmlWriter.createChunk(NAME_TAG, XmlAttribute(), animationChunk.get());
                auto animationFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), animationChunk.get());

                animationNameChunk->setStringValue(animation.first);
                animationFilenameChunk->setStringValue(animation.second->getAnimationFilename());
            }
        }
    }

    void ModelReaderWriter::loadTransformOn(Model* model, const XmlChunk* modelChunk, const XmlParser& xmlParser) {
        auto transformChunk = xmlParser.getUniqueChunk(true, TRANSFORM_TAG, XmlAttribute(), modelChunk);

        auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), transformChunk.get());
        Point3<float> position = positionChunk->getPoint3Value();

        Quaternion<float> orientation = OrientationReaderWriter::loadOrientation(transformChunk.get(), xmlParser);

        auto scaleChunk = xmlParser.getUniqueChunk(false, SCALE_TAG, XmlAttribute(), transformChunk.get());
        float scale = 1.0f;
        if (scaleChunk) {
            scale = scaleChunk->getFloatValue();
        }

        model->setTransform(Transform<float>(position, orientation, scale));
    }

    void ModelReaderWriter::writeTransformOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        auto transformChunk = xmlWriter.createChunk(TRANSFORM_TAG, XmlAttribute(), modelChunk);

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), transformChunk.get());
        positionChunk->setPoint3Value(model->getTransform().getPosition());

        OrientationReaderWriter::writeOrientation(transformChunk.get(), model->getTransform().getOrientation(), xmlWriter);

        auto scaleChunk = xmlWriter.createChunk(SCALE_TAG, XmlAttribute(), transformChunk.get());
        scaleChunk->setFloatValue(model->getTransform().getScale());
    }

    void ModelReaderWriter::loadFlagsOn(Model* model, const XmlChunk* modelChunk, const XmlParser& xmlParser) {
        auto produceShadowChunk = xmlParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, XmlAttribute(), modelChunk);
        if (produceShadowChunk) {
            model->setProduceShadow(produceShadowChunk->getBoolValue());
        } else {
            model->setProduceShadow(true);
        }
    }

    void ModelReaderWriter::writeFlagsOn(const XmlChunk* modelChunk, const Model* model, XmlWriter& xmlWriter) {
        if (!model->isProduceShadow()) {
            auto produceShadowChunk = xmlWriter.createChunk(PRODUCE_SHADOW_TAG, XmlAttribute(), modelChunk);
            produceShadowChunk->setBoolValue(model->isProduceShadow());
        }
    }

}
