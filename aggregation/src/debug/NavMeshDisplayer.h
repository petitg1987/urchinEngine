#pragma once

#include <vector>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class NavMeshDisplayer {
        public:
            NavMeshDisplayer(AIEnvironment&, Renderer3d&);
            ~NavMeshDisplayer();

            void display();
            void clearDisplay();

        private:
            std::vector<Point3<float>> toDisplayPoints(const std::vector<Point3<float>>&, float, float) const;
            void addNavMeshModel(std::shared_ptr<GeometryModel>, const Vector3<float>&);

            AIEnvironment& aiEnvironment;
            Renderer3d& renderer3d;

            std::vector<std::shared_ptr<GeometryModel>> navMeshModels;
            unsigned int loadedNavMeshId;
    };

}
