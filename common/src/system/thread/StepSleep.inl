template<class T> bool StepSleep::sleep(int sleepMs, const T* threadClass) {
    if (sleepMs < SLEEP_STEP_TIME_MS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    } else {
        int numSleeps = MathFunction::ceilToInt((float)sleepMs / (float)SLEEP_STEP_TIME_MS);
        int sleepTime = MathFunction::roundToInt((float)sleepMs / (float)numSleeps);
        for (int i = 0; i < numSleeps - 1; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            if (!threadClass->continueExecution()) {
                return true;
            }
        }
        int remainingSleepTime = sleepMs - ((numSleeps - 1) * sleepTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(remainingSleepTime));
    }
    return false;
}
