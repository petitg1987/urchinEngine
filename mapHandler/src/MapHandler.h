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

    class MapHandler { //TODO rename in MapSaveService ?
        public:
            MapHandler();

            void loadMapFromFile(const std::string&, LoadMapCallback&, const std::unique_ptr<Map>&);
            void writeMapOnFile(const std::string&, Map&) const;

            static std::string getRelativeWorkingDirectory(const std::string&);

        private:
            static constexpr char CONFIG_TAG[] = "config";
            static constexpr char WORKING_DIR_TAG[] = "relativeWorkingDirectory";
            static constexpr char SCENE_TAG[] = "scene";
    };

}
