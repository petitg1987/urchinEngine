#include "SoundTriggerReaderWriter.h"
#include "resources/sound/soundshape/SoundShapeReaderWriterRetriever.h"
#include "resources/sound/soundshape/SoundShapeReaderWriter.h"

namespace urchin
{

	SoundTriggerReaderWriter::SoundTriggerReaderWriter()
	{

	}

	SoundTriggerReaderWriter::~SoundTriggerReaderWriter()
	{

	}

	SoundTrigger *SoundTriggerReaderWriter::loadFrom(std::shared_ptr<XmlChunk> soundTriggerChunk, const XmlParser &xmlParser) const
	{
		SoundTrigger *soundTrigger = buildSoundTriggerFrom(soundTriggerChunk, xmlParser);

		return soundTrigger;
	}

	void SoundTriggerReaderWriter::writeOn(std::shared_ptr<XmlChunk> soundTriggerChunk, const SoundTrigger *soundTrigger, XmlWriter &xmlWriter) const
	{
		buildChunkFrom(soundTriggerChunk, soundTrigger, xmlWriter);
	}

	SoundTrigger *SoundTriggerReaderWriter::buildSoundTriggerFrom(std::shared_ptr<XmlChunk> soundTriggerChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> soundBehaviorChunk = xmlParser.getUniqueChunk(true, SOUND_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
		SoundBehavior soundBehavior = buildSoundBehaviorFrom(soundBehaviorChunk, xmlParser);

		std::string soundTriggerType = soundTriggerChunk->getAttributeValue(TYPE_ATTR);
		if(soundTriggerType.compare(MANUAL_VALUE)==0)
		{
			return new ManualTrigger(soundBehavior);
		}else if(soundTriggerType.compare(SHAPE_VALUE)==0)
		{
			std::shared_ptr<XmlChunk> soundShapeChunk = xmlParser.getUniqueChunk(true, SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
			std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(soundShapeChunk);
			SoundShape *soundShape = soundShapeReaderWriter->loadFrom(soundShapeChunk, xmlParser);

			return new ShapeTrigger(soundBehavior, soundShape);
		}

		throw std::invalid_argument("Unknown sound trigger type read from map: " + soundTriggerType);
	}

	void SoundTriggerReaderWriter::buildChunkFrom(std::shared_ptr<XmlChunk> soundTriggerChunk, const SoundTrigger *soundTrigger, XmlWriter &xmlWriter) const
	{
		if(soundTrigger->getTriggerType()==SoundTrigger::MANUAL_TRIGGER)
		{
			soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, MANUAL_VALUE));
		}else if(soundTrigger->getTriggerType()==SoundTrigger::SHAPE_TRIGGER)
		{
			const ShapeTrigger *shapeTrigger = static_cast<const ShapeTrigger *>(soundTrigger);
			soundTriggerChunk->setAttribute(XmlAttribute(TYPE_ATTR, SHAPE_VALUE));

			std::shared_ptr<XmlChunk> soundShapeChunk = xmlWriter.createChunk(SOUND_SHAPE_TAG, XmlAttribute(), soundTriggerChunk);
			std::shared_ptr<SoundShapeReaderWriter> soundShapeReaderWriter = SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeTrigger->getSoundShape());
			soundShapeReaderWriter->writeOn(soundShapeChunk, shapeTrigger->getSoundShape(), xmlWriter);
		}else
		{
			throw std::invalid_argument("Unknown sound trigger type to write in map: " + soundTrigger->getTriggerType());
		}

		std::shared_ptr<XmlChunk> soundBehaviorChunk = xmlWriter.createChunk(SOUND_BEHAVIOR_TAG, XmlAttribute(), soundTriggerChunk);
		buildChunkFrom(soundBehaviorChunk, soundTrigger->getSoundBehavior(), xmlWriter);
	}

	SoundBehavior SoundTriggerReaderWriter::buildSoundBehaviorFrom(std::shared_ptr<XmlChunk> soundBehaviorChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> playBehaviorChunk = xmlParser.getUniqueChunk(true, PLAY_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
		SoundBehavior::PlayBehavior playBehavior;
		if(playBehaviorChunk->getStringValue().compare(PLAY_ONCE_VALUE)==0)
		{
			playBehavior = SoundBehavior::PLAY_ONCE;
		}else if(playBehaviorChunk->getStringValue().compare(PLAY_LOOP_VALUE)==0)
		{
			playBehavior = SoundBehavior::PLAY_LOOP;
		}else
		{
			throw std::invalid_argument("Unknown play behavior read from map: " + playBehaviorChunk->getStringValue());
		}

		std::shared_ptr<XmlChunk> stopBehaviorChunk = xmlParser.getUniqueChunk(true, STOP_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
		SoundBehavior::StopBehavior stopBehavior;
		if(stopBehaviorChunk->getStringValue().compare(INSTANT_STOP_VALUE)==0)
		{
			stopBehavior = SoundBehavior::INSTANT_STOP;
		}else if(stopBehaviorChunk->getStringValue().compare(SMOOTH_STOP_VALUE)==0)
		{
			stopBehavior = SoundBehavior::SMOOTH_STOP;
		}else
		{
			throw std::invalid_argument("Unknown stop behavior read from map: " + stopBehaviorChunk->getStringValue());
		}

		std::shared_ptr<XmlChunk> volumeDecreasePercentageOnStopChunk = xmlParser.getUniqueChunk(false, VOLUME_DECREASE_PERCENTAGE_ON_STOP_TAG, XmlAttribute(), soundBehaviorChunk);
		if(volumeDecreasePercentageOnStopChunk.get()==nullptr)
		{
			return SoundBehavior(playBehavior, stopBehavior);
		}

		float volumeDecreasePercentageOnStop = volumeDecreasePercentageOnStopChunk->getFloatValue();
		return SoundBehavior(playBehavior, stopBehavior, volumeDecreasePercentageOnStop);
	}

	void SoundTriggerReaderWriter::buildChunkFrom(std::shared_ptr<XmlChunk> soundBehaviorChunk, const SoundBehavior &soundBehavior, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> playBehaviorChunk = xmlWriter.createChunk(PLAY_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
		if(soundBehavior.getPlayBehavior()==SoundBehavior::PLAY_ONCE)
		{
			playBehaviorChunk->setStringValue(PLAY_ONCE_VALUE);
		}else if(soundBehavior.getPlayBehavior()==SoundBehavior::PLAY_LOOP)
		{
			playBehaviorChunk->setStringValue(PLAY_LOOP_VALUE);
		}else
		{
			throw std::invalid_argument("Unknown play behavior to write in map: " + soundBehavior.getPlayBehavior());
		}

		std::shared_ptr<XmlChunk> stopBehaviorChunk = xmlWriter.createChunk(STOP_BEHAVIOR_TAG, XmlAttribute(), soundBehaviorChunk);
		if(soundBehavior.getStopBehavior()==SoundBehavior::INSTANT_STOP)
		{
			stopBehaviorChunk->setStringValue(INSTANT_STOP_VALUE);
		}else if(soundBehavior.getStopBehavior()==SoundBehavior::SMOOTH_STOP)
		{
			stopBehaviorChunk->setStringValue(SMOOTH_STOP_VALUE);
		}else
		{
			throw std::invalid_argument("Unknown stop behavior to write in map: " + soundBehavior.getStopBehavior());
		}

		std::shared_ptr<XmlChunk> volumeDecreasePercentageOnStopChunk = xmlWriter.createChunk(VOLUME_DECREASE_PERCENTAGE_ON_STOP_TAG, XmlAttribute(), soundBehaviorChunk);
		volumeDecreasePercentageOnStopChunk->setFloatValue(soundBehavior.getVolumeDecreasePercentageOnStop());
	}

}
