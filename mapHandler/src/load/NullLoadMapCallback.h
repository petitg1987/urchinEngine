#pragma once

#include "LoadMapCallback.h"

namespace urchin {

    class NullLoadMapCallback : public LoadMapCallback {
        public:
            void notify(LoadMapCallback::Stage, LoadMapCallback::State) override;
    };

}
