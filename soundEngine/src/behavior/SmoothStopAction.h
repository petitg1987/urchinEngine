#ifndef URCHINENGINE_SMOOTHSTOPACTION_H
#define URCHINENGINE_SMOOTHSTOPACTION_H

#include <chrono>
#include "UrchinCommon.h"

#include "behavior/SoundBehavior.h"

namespace urchin
{

    class SmoothStopAction
    {
        public:
            explicit SmoothStopAction(const SoundBehavior &);

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
