#pragma once

namespace urchin {

    class LoadMapCallback {
        public:
            enum LoadedStage {
                OBJECTS,
                LIGHTS,
                LANDSCAPE,
                SOUNDS,
                AI
            };

            virtual ~LoadMapCallback() = default;

            virtual void execute(LoadedStage) = 0;
    };

}
