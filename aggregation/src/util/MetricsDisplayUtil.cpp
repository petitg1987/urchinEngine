#include "util/MetricsDisplayUtil.h"

namespace urchin {

    MetricsDisplayUtil::MetricsDisplayUtil(const Scene& scene) :
            scene(scene),
            physicsWorld(nullptr),
            graphicsTotalDtMs(0.0f),
            physicsTotalDtMs(0.0f),
            graphicsDtMs(0.0f),
            physicsDtMs(0.0f),
            usedVideoMemoryGB(0.0f),
            totalVideoMemoryGB(0.0f),
            metricsEventCount(0) {

    }

    void MetricsDisplayUtil::setupPhysicsWorld(const PhysicsWorld* physicsWorld) {
        this->physicsWorld = physicsWorld;
    }

    float MetricsDisplayUtil::getGraphicsDtMs() const {
        return graphicsDtMs;
    }

    float MetricsDisplayUtil::getPhysicsDtMs() const {
        return physicsDtMs;
    }

    float MetricsDisplayUtil::getUsedVideoMemoryGB() const {
        return usedVideoMemoryGB;
    }

    float MetricsDisplayUtil::getTotalVideoMemoryGB() const {
        return totalVideoMemoryGB;
    }

    bool MetricsDisplayUtil::refresh() {
        bool metricsUpdated = false;

        graphicsTotalDtMs += scene.getDeltaTimeInSec() * 1000.0f;
        physicsTotalDtMs += physicsWorld != nullptr ? physicsWorld->getStepExecutionTimeInSec() * 1000.0f : 0.0f;
        metricsEventCount++;

        long deltaTimeInUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - metricsTextLastRefreshTime).count();
        if (deltaTimeInUs > REFRESH_RATE_MS * 1000) {
            graphicsDtMs = graphicsTotalDtMs / (float)metricsEventCount;
            physicsDtMs = physicsTotalDtMs / (float)metricsEventCount;
            usedVideoMemoryGB = (float)((double)scene.getUsedVideoMemory() / 1'000'000'000.0);
            totalVideoMemoryGB = (float)((double)scene.getTotalVideoMemory() / 1'000'000'000.0);

            graphicsTotalDtMs = 0.0f;
            physicsTotalDtMs = 0.0f;

            metricsTextLastRefreshTime = std::chrono::steady_clock::now();
            metricsEventCount = 0;

            metricsUpdated = true;
        }

        return metricsUpdated;
    }

}

