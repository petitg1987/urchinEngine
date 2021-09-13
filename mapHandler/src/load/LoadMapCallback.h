#pragma once

namespace urchin {

    class LoadMapCallback {
        public:
            enum Stage {
                OBJECTS, //TODO rename in model
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
