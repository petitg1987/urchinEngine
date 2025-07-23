#pragma once

#include <vector>
#include <mutex>
#include <UrchinCommon.h>

#include "collision/CollisionWorld.h"
#include "collision/ManifoldResult.h"

namespace urchin {

    /**
    * Collision visualizer: can be used to debug problems in collision process. This class stock all data coming from collision process in
    * order to be displayed in the 3D engine
    */
    class CollisionVisualizer final : public Observer {
        public:
            explicit CollisionVisualizer(CollisionWorld&);
            ~CollisionVisualizer() override;

            void notify(Observable*, int) override;

            std::vector<ManifoldResult> getManifoldResults() const;

        private:
            CollisionWorld& collisionWorld;
            mutable std::mutex visualizerDataMutex;

            std::vector<ManifoldResult> manifoldResults;
    };

}
