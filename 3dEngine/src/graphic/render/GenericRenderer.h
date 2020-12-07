#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/render/model/DataType.h"
#include "graphic/render/model/DataDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/texture/TextureReader.h"

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer {
        public:
            static const unsigned int PRIMITIVE_RESTART_INDEX_VALUE;

            friend class RenderTarget;

            struct Data {
                DataType dataType = DataType::FLOAT;
                DataDimension dataDimension = DataDimension::TWO_DIMENSION;
                const void *ptr = nullptr;
                std::size_t dataCount = 0;
            };

            struct Indices {
                const void *ptr = nullptr;
                std::size_t indicesCount = 0;
                bool hasPrimitiveRestartIndex = false;
            };

            explicit GenericRenderer(const GenericRendererBuilder*);
            ~GenericRenderer();

            void updateData(std::size_t, const std::vector<Point2<float>>*);
            void updateData(std::size_t, const std::vector<Point3<float>>*);
            void updateData(std::size_t, const std::vector<Vector3<float>>*);

            void updateTexture(std::size_t, const TextureReader&);
            unsigned int addAdditionalTexture(const TextureReader&);
            void clearAdditionalTextures();
            void renderTextures(bool);

        protected:
            void render() const;

        private:
            std::size_t computeVerticesCount() const;
            void initializeTexture(const TextureReader&) const;
            void initializeDisplay();
            void sendData(std::size_t, bool);

            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int dataTypeToSize(DataType) const;
            unsigned int dataTypeToGlType(DataType) const;
            unsigned int dataDimensionToSize(DataDimension) const;

            void resetRenderDefaultValues() const;

            ShapeType shapeType;

            std::vector<Data> data;
            Indices indices;
            std::size_t verticesCount;

            bool transparencyEnabled;

            bool depthTestEnabled;

            bool cullFaceEnabled;

            PolygonMode polygonMode;
            float outlineSize;

            std::vector<TextureReader> textureReaders, additionalTextureReaders;
            bool bNeedRenderTextures;

            unsigned int vertexArrayObject;
            std::vector<unsigned int> bufferIds;
    };

}

#endif

