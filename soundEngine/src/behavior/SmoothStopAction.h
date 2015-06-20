#ifndef ENGINE_SMOOTHSTOPACTION_H
#define ENGINE_SMOOTHSTOPACTION_H

#include <chrono>
#include "UrchinCommon.h"

#include "behavior/SoundBehavior.h"

namespace urchin
{

	class SmoothStopAction
	{
		public:
			SmoothStopAction(const SoundBehavior &);
			~SmoothStopAction();

			void startSmoothStop();
			void endSmoothStop();
			bool isSmoothStopStarted() const;

			float computeChangeVolumePercentage();
			float getTotalChangeVolumePercentage() const;
			bool isSmoothStopProcessing() const;

		private:
			SoundBehavior soundBehavior;

			std::chrono::high_resolution_clock::time_point startingTime;
			std::chrono::high_resolution_clock::time_point previousTime;

			bool bIsSmoothStopStarted;
			float totalChangeVolumePercentage;
	};

}

#endif
