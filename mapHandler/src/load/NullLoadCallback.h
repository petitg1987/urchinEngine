#pragma once

#include "LoadCallback.h"

namespace urchin {

    class NullLoadCallback : public LoadCallback {
        public:
            void execute(LoadedStage) override;
    };

}
