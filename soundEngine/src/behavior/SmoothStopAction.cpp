#include <cassert>

#include "behavior/SmoothStopAction.h"

namespace urchin
{

	SmoothStopAction::SmoothStopAction(const SoundBehavior &soundBehavior) :
		soundBehavior(soundBehavior),
		bIsSmoothStopStarted(false),
		totalChangeVolumePercentage(0.0)
	{

	}

	SmoothStopAction::~SmoothStopAction()
	{

	}

	void SmoothStopAction::startSmoothStop()
	{
		startingTime = std::chrono::high_resolution_clock::now();
		previousTime = startingTime;

		totalChangeVolumePercentage = 0.0;

		bIsSmoothStopStarted = true;
	}

	void SmoothStopAction::endSmoothStop()
	{
		bIsSmoothStopStarted = false;
	}

	bool SmoothStopAction::isSmoothStopStarted() const
	{
		return bIsSmoothStopStarted;
	}

	/**
	 * @param Volume percentage to change (value from 0.0 to 1.0)
	 */
	float SmoothStopAction::computeChangeVolumePercentage()
	{
		#ifdef _DEBUG
			assert(bIsSmoothStopStarted);
		#endif

		if(soundBehavior.getStopBehavior()!=SoundBehavior::SMOOTH_STOP)
		{
			return 0.0;
		}

		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		int timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();
		previousTime = currentTime;

		float changeVolumePercentage = - (static_cast<float>(timeInterval) / 1000.0) * soundBehavior.getVolumeDecreasePercentageOnStop();
		totalChangeVolumePercentage += changeVolumePercentage;

		return changeVolumePercentage;
	}

	/**
	 * @return Total computed change volume percentage from start
	 */
	float SmoothStopAction::getTotalChangeVolumePercentage() const
	{
		return totalChangeVolumePercentage;
	}

	/**
	 * @return True if sound volume is perceptible
	 */
	bool SmoothStopAction::isSmoothStopProcessing() const
	{
		#ifdef _DEBUG
			assert(bIsSmoothStopStarted);
		#endif

		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		int timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startingTime).count();

		int nbMillisecondToVolumeReachZero = (1.0 / soundBehavior.getVolumeDecreasePercentageOnStop()) * 1000.0;

		return timeInterval < nbMillisecondToVolumeReachZero;
	}

}
