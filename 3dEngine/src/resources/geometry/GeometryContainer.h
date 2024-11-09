#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class GeometryContainer {
        public:
            explicit GeometryContainer(RenderTarget&);

            void addGeometry(std::shared_ptr<GeometryModel>);
            void removeGeometry(const GeometryModel&);

            void prepareRendering(unsigned int&, const Matrix4<float>&) const;

        private:
            RenderTarget& renderTarget;

            std::vector<std::shared_ptr<GeometryModel>> geometryModels;
    };

}
