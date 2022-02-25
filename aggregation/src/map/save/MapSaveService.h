#pragma once

#include <string>
#include <map>
#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <map/Map.h>
#include <map/save/LoadMapCallback.h>

namespace urchin {

    class MapSaveService {
        public:
            void loadMap(const std::string&, LoadMapCallback&, Map&) const;
            void saveMap(const std::string&, const Map&) const;

            static std::string getRelativeWorkingDirectory(std::string);

        private:
            void loadMap(Map&, const UdaChunk*, const UdaParser&, LoadMapCallback&) const;
            void loadObjectEntities(Map&, const UdaChunk*, const UdaParser&) const;
            void loadLightEntities(Map& p, const UdaChunk*, const UdaParser&) const;
            void loadTerrainEntities(Map&, const UdaChunk*, const UdaParser&) const;
            void loadWaterEntities(Map&, const UdaChunk*, const UdaParser&) const;
            void loadSkyEntity(Map&, const UdaChunk*, const UdaParser&) const;
            void loadSoundEntities(Map&, const UdaChunk*, const UdaParser&) const;
            void loadAIConfig(const Map&, const UdaChunk*, const UdaParser&) const;

            void writeMap(const Map&, UdaChunk&, UdaWriter&) const;
            void writeObjectEntities(const Map&, UdaChunk&, UdaWriter&) const;
            void writeLightEntities(const Map&, UdaChunk&, UdaWriter&) const;
            void writeTerrainEntities(const Map&, UdaChunk&, UdaWriter&) const;
            void writeWaterEntities(const Map&, UdaChunk&, UdaWriter&) const;
            void writeSkyEntity(const Map&, UdaChunk&, UdaWriter&) const;
            void writeSoundEntities(const Map& , UdaChunk&, UdaWriter&) const;
            void writeAIConfig(const Map&, UdaChunk&, UdaWriter&) const;

            static constexpr char CONFIG_TAG[] = "config";
            static constexpr char WORKING_DIR_TAG[] = "relativeWorkingDirectory";
            static constexpr char SCENE_TAG[] = "scene";

            static constexpr char OBJECTS_TAG[] = "objects";
            static constexpr char OBJECT_TAG[] = "object";
            static constexpr char LIGHTS_TAG[] = "lights";
            static constexpr char LIGHT_TAG[] = "light";
            static constexpr char TERRAINS_TAG[] = "terrains";
            static constexpr char TERRAIN_TAG[] = "terrain";
            static constexpr char WATERS_TAG[] = "waters";
            static constexpr char WATER_TAG[] = "water";
            static constexpr char SKY_TAG[] = "sky";
            static constexpr char SOUND_ELEMENTS_TAG[] = "soundElements";
            static constexpr char SOUND_ELEMENT_TAG[] = "soundElement";
            static constexpr char AI_ELEMENTS_TAG[] = "aiElements";
    };

}
