#ifndef URCHINENGINE_MESH_H
#define URCHINENGINE_MESH_H

#include "UrchinCommon.h"

#include "resources/model/ConstMesh.h"
#include "scene/renderer3d/model/displayer/MeshParameter.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/TargetRenderer.h"

namespace urchin {

    class Mesh {
        public:
            explicit Mesh(const ConstMesh *);

            void update(const std::vector<Bone> &);

            void display(const TargetRenderer *, const MeshParameter &) const;
            void drawBaseBones(const TargetRenderer *, const Matrix4<float> &, const Matrix4<float> &) const;

        private:
            const ConstMesh *const constMesh;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<Vector3<float>> tangents;

            std::unique_ptr<GenericRenderer> meshRenderer;
    };

}

#endif
