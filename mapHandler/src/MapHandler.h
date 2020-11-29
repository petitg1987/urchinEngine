#ifndef URCHINENGINE_MAPHANDLER_H
#define URCHINENGINE_MAPHANDLER_H

#include <string>
#include <map>
#include <memory>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinAIEngine.h"
#include "resources/Map.h"
#include "load/LoadCallback.h"
#include "load/NullLoadCallback.h"

namespace urchin {

    /**
    * A map handler for 3d objects, physics, sounds...
    */
    class MapHandler {
        //XML tags
        #define SCENE_TAG "scene"

        //XML attributes
        #define WORKING_DIR_ATTR "relativeWorkingDirectory"

        public:
            MapHandler(Renderer3d*, PhysicsWorld*, SoundManager*, AIManager*);
            ~MapHandler();

            void loadMapFromFile(const std::string&, LoadCallback&);
            void writeMapOnFile(const std::string&) const;

            static std::string getRelativeWorkingDirectory(const std::string&);
            std::string getRelativeWorkingDirectory() const;
            void setRelativeWorkingDirectory(const std::string&);

            void refreshMap();
            Map *getMap() const;

            void pause();
            void unpause();

        private:
            std::string relativeWorkingDirectory;
            Map *map;
    };

}

#endif
