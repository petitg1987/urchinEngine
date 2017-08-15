#ifndef ENGINE_MANUALTRIGGER_H
#define ENGINE_MANUALTRIGGER_H

#include "trigger/SoundTrigger.h"

namespace urchin
{

	/**
	* Trigger performed by the user
	*/
	class ManualTrigger : public SoundTrigger
	{
		public:
			explicit ManualTrigger(const SoundBehavior &);

			SoundTrigger::TriggerType getTriggerType() const override;

			void play();
			void stop();
			void pause();

			SoundTrigger::TriggerResultValue evaluateTrigger(const Point3<float> &) override;

		private:
			enum ManualTriggerValue
			{
				NO_TRIGGER,

				PLAY,
				PLAY_LOOP,
				STOP,
				SMOOTH_STOP,
				PAUSE
			};

			ManualTriggerValue manualTriggerValue;
	};

}

#endif
