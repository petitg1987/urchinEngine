#pragma once

#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>
#include <resources/model/ConstMesh.h>

namespace urchin {

    class ModelBuilder {
        public:
            explicit ModelBuilder(std::shared_ptr<Material>);
            explicit ModelBuilder(const std::string&);

            std::unique_ptr<Model> newModel(const std::string&, const std::vector<Point3<float>>&, const std::vector<unsigned int>&, const std::vector<Point2<float>>&) const;

        private:
            std::unique_ptr<const ConstMesh> buildConstMesh(const std::vector<Point3<float>>&, const std::vector<unsigned int>&, const std::vector<Point2<float>>&) const;

            std::shared_ptr<Material> material;
    };

}