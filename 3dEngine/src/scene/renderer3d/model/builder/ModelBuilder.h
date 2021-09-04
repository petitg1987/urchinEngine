#pragma once

#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>
#include <resources/model/ConstMesh.h>

namespace urchin {

    class ModelBuilder {
        public:
            explicit ModelBuilder(std::string);

            std::unique_ptr<Model> newModel(const std::string&, const std::vector<Point3<float>>&, const std::vector<unsigned int>&, const std::vector<Point2<float>>&) const;

        private:
            std::unique_ptr<const ConstMesh> buildConstMesh(const std::vector<Point3<float>>&, const std::vector<unsigned int>&, const std::vector<Point2<float>>&) const;

            std::string materialFilename;
    };

}