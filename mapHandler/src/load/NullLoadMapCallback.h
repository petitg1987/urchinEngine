#pragma once

#include "LoadMapCallback.h"

namespace urchin {

    class NullLoadMapCallback : public LoadMapCallback {
        public:
            void execute(LoadedStage) override;
    };

}
