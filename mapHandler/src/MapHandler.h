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
            void loadFrom(Map&, const UdaChunk*, const UdaParser&, LoadMapCallback&); //TODO rename
            void loadObjectEntities(Map&, const UdaChunk*, const UdaParser&);
            void loadLightEntities(Map& p, const UdaChunk*, const UdaParser&);
            void loadTerrainEntities(Map&, const UdaChunk*, const UdaParser&);
            void loadWaterEntities(Map&, const UdaChunk*, const UdaParser&);
            void loadSkyEntity(Map&, const UdaChunk*, const UdaParser&);
            void loadSoundEntities(Map&, const UdaChunk*, const UdaParser&);
            void loadAIConfig(Map&, const UdaChunk*, const UdaParser&);

            void writeOn(Map&, UdaChunk&, UdaWriter&) const; //TODO rename
            void writeObjectEntities(Map&, UdaChunk&, UdaWriter&) const;
            void writeLightEntities(Map&, UdaChunk&, UdaWriter&) const;
            void writeTerrainEntities(Map&, UdaChunk&, UdaWriter&) const;
            void writeWaterEntities(Map&, UdaChunk&, UdaWriter&) const;
            void writeSkyEntity(Map&, UdaChunk&, UdaWriter&) const;
            void writeSoundEntities(Map& , UdaChunk&, UdaWriter&) const;
            void writeAIConfig(Map&, UdaChunk&, UdaWriter&) const;

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
