#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

namespace urchin {

    class TerrainReaderWriter {
        //XML attributes
        static constexpr char INDEX_ATTR[] = "index";

        //XML tags
        static constexpr char MESH_TAG[] = "mesh";
        static constexpr char HEIGHT_FILENAME_TAG[] = "heightFilename";
        static constexpr char XZ_SCALE_TAG[] = "xzScale";
        static constexpr char Y_SCALE_TAG[] = "yScale";
        static constexpr char MATERIAL_TAG[] = "material";
        static constexpr char MASK_MAP_FILENAME[] = "maskMapFilename";
        static constexpr char S_REPEAT_TAG[] = "sRepeat";
        static constexpr char T_REPEAT_TAG[] = "tRepeat";
        static constexpr char MATERIAL_FILENAMES[] = "materialFilenames";
        static constexpr char MATERIAL_FILENAME[] = "materialFilename";
        static constexpr char POSITION_TAG[] = "position";
        static constexpr char AMBIENT_TAG[] = "ambient";
        static constexpr char GRASS_TAG[] = "grass";
        static constexpr char GRASS_TEXTURE_FILENAME_TAG[] = "grassTextureFilename";
        static constexpr char GRASS_MASK_FILENAME_TAG[] = "grassMaskFilename";
        static constexpr char NUM_GRASS_IN_TEX_TAG[] = "numGrassInTex";
        static constexpr char GRASS_HEIGHT_TAG[] = "grassHeight";
        static constexpr char GRASS_LENGTH_TAG[] = "grassLength";
        static constexpr char GRASS_QUANTITY_TAG[] = "grassQuantity";
        static constexpr char WIND_DIRECTION_TAG[] = "windDirection";
        static constexpr char WIND_STRENGTH_TAG[] = "windStrength";

        public:
            Terrain* loadFrom(const DataChunk*, const DataParser&) const;
            void writeOn(DataChunk*, const Terrain*, DataWriter&) const;

        private:
            Terrain* buildTerrainFrom(const DataChunk*, const DataParser&) const;
            void buildChunkFrom(const DataChunk*, const Terrain*, DataWriter&) const;

            void loadPropertiesOn(Terrain*, const DataChunk*, const DataParser&) const;
            void writePropertiesOn(const DataChunk*, const Terrain*, DataWriter&) const;

            void loadGrassOn(Terrain*, const DataChunk*, const DataParser&) const;
            void writeGrassOn(const DataChunk*, const Terrain*, DataWriter&) const;
    };

}
