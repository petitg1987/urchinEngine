#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/render/model/DataType.h"
#include "graphic/render/model/DataDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/render/model/Texture.h"

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer {
        public:
            struct Data {
                DataType dataType = DataType::FLOAT;
                DataDimension dataDimension = DataDimension::TWO_DIMENSION;
                const void *ptr = nullptr;
                unsigned int dataCount = 0;
            };

            struct Indices {
                const void *ptr = nullptr;
                unsigned int indicesCount = 0;
            };

            explicit GenericRenderer(const GenericRendererBuilder *);
            ~GenericRenderer();

            void updateData(std::size_t, const std::vector<Point2<float>> *);
            void updateData(std::size_t, const std::vector<Point3<float>> *);
            void updateData(std::size_t, const std::vector<Vector3<float>> *);

            void updateTexture(std::size_t, Texture);
            unsigned int addAdditionalTexture(Texture);
            void clearAdditionalTextures();
            void renderTextures(bool);

            void draw() const;

        private:
            unsigned int computeVerticesCount() const;
            void initializeTexture(Texture) const;
            void initializeDisplay();
            void sendData(std::size_t, bool);

            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int dataTypeToSize(DataType) const;
            unsigned int dataTypeToGlType(DataType) const;
            unsigned int dataDimensionToSize(DataDimension) const;

            void resetDrawDefaultValues() const;

            ShapeType shapeType;

            std::vector<Data> data;
            Indices indices;
            unsigned int verticesCount;

            bool transparencyEnabled;

            bool depthTestEnabled;

            bool cullFaceEnabled;

            PolygonMode polygonMode;
            float outlineSize;

            std::vector<Texture> textures, additionalTextures;
            bool bNeedRenderTextures;

            unsigned int vertexArrayObject;
            std::vector<unsigned int> bufferIds;
    };

}

#endif

