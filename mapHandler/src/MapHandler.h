#pragma once

#include <string>
#include <map>
#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <resources/Map.h>
#include <load/LoadMapCallback.h>

namespace urchin {

    /**
    * A map handler for 3d objects, physics, sounds...
    */
    class MapHandler {
        public:
            MapHandler(Renderer3d*, PhysicsWorld*, SoundEnvironment*, AIEnvironment*);

            void loadMapFromFile(const std::string&, LoadMapCallback&);
            void writeMapOnFile(const std::string&) const;

            static std::string getRelativeWorkingDirectory(const std::string&);
            std::string getRelativeWorkingDirectory() const;
            void setRelativeWorkingDirectory(const std::string&);

            void refreshMap();
            Map& getMap() const;

            void pause();
            void unpause();

        private:
            static constexpr char CONFIG_TAG[] = "config";
            static constexpr char WORKING_DIR_TAG[] = "relativeWorkingDirectory";
            static constexpr char SCENE_TAG[] = "scene";

            std::string relativeWorkingDirectory;
            std::unique_ptr<Map> map;
    };

}
