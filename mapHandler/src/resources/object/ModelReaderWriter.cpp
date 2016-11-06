#include <vector>

#include "ModelReaderWriter.h"
#include "resources/common/OrientationReaderWriter.h"

namespace urchin
{

	ModelReaderWriter::ModelReaderWriter()
	{

	}

	ModelReaderWriter::~ModelReaderWriter()
	{

	}

	Model *ModelReaderWriter::loadFrom(std::shared_ptr<XmlChunk> modelChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> meshChunk = xmlParser.getUniqueChunk(true, MESH_TAG, XmlAttribute(), modelChunk);
		std::shared_ptr<XmlChunk> meshFilenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), meshChunk);

		Model *model = new Model(meshFilenameChunk->getStringValue());
		loadAnimationsOn(model, modelChunk, xmlParser);
		loadTransformOn(model, modelChunk, xmlParser);
		loadFlagsOn(model, modelChunk, xmlParser);

		return model;
	}

	void ModelReaderWriter::writeOn(std::shared_ptr<XmlChunk> modelChunk, const Model *model, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> meshChunk = xmlWriter.createChunk(MESH_TAG, XmlAttribute(), modelChunk);
		std::shared_ptr<XmlChunk> meshFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), meshChunk);

		meshFilenameChunk->setStringValue(model->getMeshes()->getMeshFilename());
		writeAnimationsOn(modelChunk, model, xmlWriter);
		writeTransformOn(modelChunk, model, xmlWriter);
		writeFlagsOn(modelChunk, model, xmlWriter);
	}

	void ModelReaderWriter::loadAnimationsOn(Model *model, std::shared_ptr<XmlChunk> modelChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> animationsListChunk = xmlParser.getUniqueChunk(false, ANIMATIONS_TAG, XmlAttribute(), modelChunk);
		if(animationsListChunk.get()!=nullptr)
		{
			std::vector<std::shared_ptr<XmlChunk>> animationsChunk = xmlParser.getChunks(ANIMATION_TAG, XmlAttribute(), animationsListChunk);
			for(unsigned int i=0; i<animationsChunk.size(); ++i)
			{
				std::shared_ptr<XmlChunk> animationNameChunk = xmlParser.getUniqueChunk(true, NAME_TAG, XmlAttribute(), animationsChunk[i]);
				std::shared_ptr<XmlChunk> animationFilenameChunk = xmlParser.getUniqueChunk(true, FILENAME_TAG, XmlAttribute(), animationsChunk[i]);

				model->loadAnimation(animationNameChunk->getStringValue(), animationFilenameChunk->getStringValue());
			}
		}
	}

	void ModelReaderWriter::writeAnimationsOn(std::shared_ptr<XmlChunk> modelChunk, const Model *model, XmlWriter &xmlWriter) const
	{
		std::map<std::string, const ConstAnimation *> animations = model->getAnimations();
		if(animations.size()>0)
		{
			std::shared_ptr<XmlChunk> animationsListChunk = xmlWriter.createChunk(ANIMATIONS_TAG, XmlAttribute(), modelChunk);
			for(std::map<std::string, const ConstAnimation *>::const_iterator it = animations.begin(); it!=animations.end(); ++it)
			{
				std::shared_ptr<XmlChunk> animationChunk = xmlWriter.createChunk(ANIMATION_TAG, XmlAttribute(), animationsListChunk);

				std::shared_ptr<XmlChunk> animationNameChunk = xmlWriter.createChunk(NAME_TAG, XmlAttribute(), animationChunk);
				std::shared_ptr<XmlChunk> animationFilenameChunk = xmlWriter.createChunk(FILENAME_TAG, XmlAttribute(), animationChunk);

				animationNameChunk->setStringValue(it->first);
				animationFilenameChunk->setStringValue(it->second->getAnimationFilename());
			}
		}
	}

	void ModelReaderWriter::loadTransformOn(Model *model, std::shared_ptr<XmlChunk> modelChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> transformChunk = xmlParser.getUniqueChunk(true, TRANSFORM_TAG, XmlAttribute(), modelChunk);

		std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), transformChunk);
		Point3<float> position = positionChunk->getPoint3Value();

		Quaternion<float> orientation = OrientationReaderWriter().loadOrientation(transformChunk, xmlParser);

		std::shared_ptr<XmlChunk> scaleChunk = xmlParser.getUniqueChunk(false, SCALE_TAG, XmlAttribute(), transformChunk);
		float scale = 1.0f;
		if(scaleChunk.get()!=nullptr)
		{
			scale = scaleChunk->getFloatValue();
		}

		model->setTransform(Transform<float>(position, orientation, scale));
	}

	void ModelReaderWriter::writeTransformOn(std::shared_ptr<XmlChunk> modelChunk, const Model *model, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> transformChunk = xmlWriter.createChunk(TRANSFORM_TAG, XmlAttribute(), modelChunk);

		std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), transformChunk);
		positionChunk->setPoint3Value(model->getTransform().getPosition());

		OrientationReaderWriter().writeOrientation(transformChunk, model->getTransform().getOrientation(), xmlWriter);

		std::shared_ptr<XmlChunk> scaleChunk = xmlWriter.createChunk(SCALE_TAG, XmlAttribute(), transformChunk);
		scaleChunk->setFloatValue(model->getTransform().getScale());
	}

	void ModelReaderWriter::loadFlagsOn(Model *model, std::shared_ptr<XmlChunk> modelChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> produceShadowChunk = xmlParser.getUniqueChunk(false, PRODUCE_SHADOW_TAG, XmlAttribute(), modelChunk);
		if(produceShadowChunk.get()!=nullptr)
		{
			model->setProduceShadow(produceShadowChunk->getBoolValue());
		}else
		{
			model->setProduceShadow(true);
		}
	}

	void ModelReaderWriter::writeFlagsOn(std::shared_ptr<XmlChunk> modelChunk, const Model *model, XmlWriter &xmlWriter) const
	{
		if(!model->isProduceShadow())
		{
			std::shared_ptr<XmlChunk> produceShadowChunk = xmlWriter.createChunk(PRODUCE_SHADOW_TAG, XmlAttribute(), modelChunk);
			produceShadowChunk->setBoolValue(model->isProduceShadow());
		}
	}

}
