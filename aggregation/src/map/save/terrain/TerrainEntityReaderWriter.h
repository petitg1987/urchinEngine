#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include "model/TerrainEntity.h"

namespace urchin {

    class TerrainEntityReaderWriter {
        public:
            static std::unique_ptr<TerrainEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const TerrainEntity&, UdaParser&);

        private:
            static std::unique_ptr<Terrain> buildTerrain(const UdaChunk*, const UdaParser&);
            static void writeTerrainChunk(UdaChunk&, const Terrain&, UdaParser&);

            static void loadProperties(Terrain&, const UdaChunk*, const UdaParser&);
            static void writeProperties(UdaChunk&, const Terrain&, UdaParser&);

            static constexpr char NAME_ATTR[] = "name";
            static constexpr char INDEX_ATTR[] = "index";

            static constexpr char MESH_TAG[] = "mesh";
            static constexpr char HEIGHT_FILENAME_TAG[] = "heightFilename";
            static constexpr char XZ_SCALE_TAG[] = "xzScale";
            static constexpr char Y_SCALE_TAG[] = "yScale";
            static constexpr char FLAT_MODE_TAG[] = "flatMode";
            static constexpr char MATERIAL_TAG[] = "material";
            static constexpr char MASK_MAP_FILENAME[] = "maskMapFilename";
            static constexpr char S_REPEAT_TAG[] = "sRepeat";
            static constexpr char T_REPEAT_TAG[] = "tRepeat";
            static constexpr char MATERIAL_FILENAMES[] = "materialFilenames";
            static constexpr char MATERIAL_FILENAME[] = "materialFilename";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char AMBIENT_TAG[] = "ambient";
            static constexpr char LIGHT_MASK_TAG[] = "lightMask";
    };

}
