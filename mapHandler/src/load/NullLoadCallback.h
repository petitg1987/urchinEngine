#ifndef URCHINENGINE_NULLCALLBACK_H
#define URCHINENGINE_NULLCALLBACK_H

#include "LoadCallback.h"

namespace urchin {

    class NullLoadCallback : public LoadCallback {
        public:
            void execute(LoadedStage) override;
    };

}

#endif
