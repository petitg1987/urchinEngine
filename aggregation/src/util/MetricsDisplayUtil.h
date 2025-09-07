#pragma once

#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class MetricsDisplayUtil {
        public:
            explicit MetricsDisplayUtil(const Scene&);

            void setupPhysicsWorld(const PhysicsWorld*);

            float getGraphicsDtMs() const;
            float getPhysicsDtMs() const;
            float getUsedVideoMemoryGB() const;
            float getTotalVideoMemoryGB() const;

            bool refresh();

        private:
            static constexpr unsigned int REFRESH_RATE_MS = 200;

            const Scene& scene;
            const PhysicsWorld* physicsWorld;

            float graphicsTotalDtMs;
            float physicsTotalDtMs;

            float graphicsDtMs;
            float physicsDtMs;
            float usedVideoMemoryGB;
            float totalVideoMemoryGB;

            unsigned int metricsEventCount;
            std::chrono::steady_clock::time_point metricsTextLastRefreshTime;

    };

}

