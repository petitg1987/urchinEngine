#pragma once

namespace urchin {

    class LoadMapCallback {
        public:
            enum Stage {
                OBJECTS,
                LIGHTS,
                LANDSCAPE,
                SOUNDS,
                AI
            };

            enum State {
                START_LOADING,
                LOADED
            };

            virtual ~LoadMapCallback() = default;

            virtual void notify(Stage, State);
    };

}
