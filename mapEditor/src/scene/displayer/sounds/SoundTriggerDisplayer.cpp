#include <memory>
#include <stdexcept>
#include "UrchinCommon.h"

#include "SoundTriggerDisplayer.h"

namespace urchin
{

	SoundTriggerDisplayer::SoundTriggerDisplayer(SceneManager *sceneManager) :
		sceneManager(sceneManager)
	{

	}

	SoundTriggerDisplayer::~SoundTriggerDisplayer()
	{
		cleanCurrentDisplay();
	}

	void SoundTriggerDisplayer::displaySoundTriggerFor(const SceneSound *sceneSound)
	{
		cleanCurrentDisplay();

		if(sceneSound)
		{
			const SoundTrigger *soundTrigger = sceneSound->getSoundTrigger();

			if(const ShapeTrigger *shapeTrigger = dynamic_cast<const ShapeTrigger *>(soundTrigger))
			{
				GeometryModel *geometryModel = retrieveGeometry(shapeTrigger->getSoundShape());
				soundTriggerModels.push_back(geometryModel);
			}

			for(unsigned int i=0; i<soundTriggerModels.size(); ++i)
			{
				soundTriggerModels[i]->setColor(0.0, 1.0, 1.0);
				sceneManager->getActiveRenderer3d()->getGeometryManager()->addGeometry(soundTriggerModels[i]);
			}
		}
	}

	GeometryModel *SoundTriggerDisplayer::retrieveGeometry(const SoundShape *soundShape)
	{
		SoundShape::ShapeType shapeType = soundShape->getShapeType();
		if(shapeType==SoundShape::ShapeType::SPHERE_SHAPE)
		{
			return retrieveSphereGeometry(soundShape);
		}else if(shapeType==SoundShape::ShapeType::BOX_SHAPE)
		{
			return retrieveBoxGeometry(soundShape);
		}

		throw std::invalid_argument("Unknown shape type to retrieve geometry: " + std::to_string(shapeType));
	}

	GeometryModel *SoundTriggerDisplayer::retrieveSphereGeometry(const SoundShape *soundShape) const
	{
		const SoundSphere *soundSphere = static_cast<const SoundSphere *>(soundShape);
		return new SphereModel(soundSphere->getPlayTriggerSphere(), 15);
	}

	GeometryModel *SoundTriggerDisplayer::retrieveBoxGeometry(const SoundShape *soundShape) const
	{
		const SoundBox *soundBox = static_cast<const SoundBox *>(soundShape);
		return new OBBoxModel(soundBox->getPlayTriggerBox());
	}

	void SoundTriggerDisplayer::cleanCurrentDisplay()
	{
		for(unsigned int i=0; i<soundTriggerModels.size(); ++i)
		{
			sceneManager->getActiveRenderer3d()->getGeometryManager()->removeGeometry(soundTriggerModels[i]);
			delete soundTriggerModels[i];
		}

		soundTriggerModels.clear();
	}
}
