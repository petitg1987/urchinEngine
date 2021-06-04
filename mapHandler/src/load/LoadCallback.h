#pragma once

namespace urchin {

    class LoadCallback {
        public:
            enum LoadedStage {
                OBJECTS,
                LIGHTS,
                LANDSCAPE,
                SOUNDS,
                AI
            };

            virtual ~LoadCallback() = default;

            virtual void execute(LoadedStage) = 0;
    };

}
