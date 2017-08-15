#ifndef ENGINE_SHAPETRIGGER_H
#define ENGINE_SHAPETRIGGER_H

#include "trigger/SoundTrigger.h"
#include "trigger/shape/SoundShape.h"

namespace urchin
{

	/**
	* Trigger performed when inside a shape
	*/
	class ShapeTrigger : public SoundTrigger
	{
		public:
			ShapeTrigger(const SoundBehavior &, const SoundShape *);
			~ShapeTrigger() override;

			SoundTrigger::TriggerType getTriggerType() const override;

			SoundTrigger::TriggerResultValue evaluateTrigger(const Point3<float> &) override;

			const SoundShape *getSoundShape() const;

		private:
			SoundTrigger::TriggerResultValue getPlayTriggerValue();
			SoundTrigger::TriggerResultValue getStopTriggerValue();

			const SoundShape *soundShape;

			bool isPlaying;
	};

}

#endif
