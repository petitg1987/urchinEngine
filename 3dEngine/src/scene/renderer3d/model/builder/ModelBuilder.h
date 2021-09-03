#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>
#include <resources/model/ConstMesh.h>

namespace urchin {

    class ModelBuilder {
        public:
            std::unique_ptr<Model> buildModel(const std::string&, const std::vector<Point3<float>>&, const std::vector<unsigned int>&, const std::vector<Point2<float>>&) const;

        private:
            std::unique_ptr<const ConstMesh> buildConstMesh(const std::string&, const std::vector<Point3<float>>&, const std::vector<unsigned int>&,
                    const std::vector<Point2<float>>&) const;
    };

}