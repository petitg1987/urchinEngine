#ifndef URCHINENGINE_MLOADCALLBACK_H
#define URCHINENGINE_MLOADCALLBACK_H

namespace urchin
{

    class LoadCallback
    {
        public:
            enum LoadedStage
            {
                OBJECTS,
                LIGHTS,
                TERRAINS,
                SOUNDS,
                AI
            };

            virtual void execute(LoadedStage) = 0;
    };

}

#endif
