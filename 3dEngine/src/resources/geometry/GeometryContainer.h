#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <api/render/target/RenderTarget.h>
#include <resources/geometry/GeometryModel.h>
#include <scene/renderer3d/camera/Camera.h>

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
