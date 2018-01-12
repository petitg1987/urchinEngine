#ifndef URCHINENGINE_TERRAINREADERWRITER_H
#define URCHINENGINE_TERRAINREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin
{

    class TerrainReaderWriter
    {
        //XML attributes
        #define INDEX_ATTR "index"

        //XML tags
        #define MESH_TAG "mesh"
        #define HEIGHT_FILENAME_TAG "heightFilename"
        #define XZ_SCALE_TAG "xzScale"
        #define Y_SCALE_TAG "yScale"
        #define MATERIAL_TAG "material"
        #define MASK_MAP_FILENAME "maskMapFilename"
        #define S_REPEAT_TAG "sRepeat"
        #define T_REPEAT_TAG "tRepeat"
        #define MATERIAL_FILENAMES "materialFilenames"
        #define MATERIAL_FILENAME "materialFilename"
        #define POSITION_TAG "position"
        #define AMBIENT_TAG "ambient"
        #define GRASS_TAG "grass"
        #define GRASS_TEXTURE_FILENAME_TAG "grassTextureFilename"
        #define GRASS_MASK_FILENAME_TAG "grassMaskFilename"
        #define NUM_GRASS_IN_TEX_TAG "numGrassInTex"
        #define GRASS_HEIGHT_TAG "grassHeight"
        #define GRASS_LENGTH_TAG "grassLength"
        #define GRASS_OFFSET_TAG "grassOffset"
        #define WIND_DIRECTION_TAG "windDirection"
        #define WIND_STRENGTH_TAG "windStrength"

        public:
            Terrain *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writeOn(std::shared_ptr<XmlChunk>, const Terrain *, XmlWriter &) const;

        private:
            Terrain *buildTerrainFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void buildChunkFrom(std::shared_ptr<XmlChunk>, const Terrain *, XmlWriter &) const;

            void loadPropertiesOn(Terrain *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writePropertiesOn(std::shared_ptr<XmlChunk>, const Terrain *, XmlWriter &) const;

            void loadGrassOn(Terrain *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
            void writeGrassOn(std::shared_ptr<XmlChunk>, const Terrain *, XmlWriter &) const;
    };

}

#endif
