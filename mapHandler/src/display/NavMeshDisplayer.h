#ifndef URCHINENGINE_NAVMESHDISPLAYER_H
#define URCHINENGINE_NAVMESHDISPLAYER_H

#include <vector>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinAIEngine.h"

namespace urchin {

    class NavMeshDisplayer {
        public:
            NavMeshDisplayer(AIManager *, Renderer3d *);
            ~NavMeshDisplayer();

            void display();
            void clearDisplay();

        private:
            std::vector<Point3<float>> toDisplayPoints(const std::vector<Point3<float>> &, float) const;
            void addNavMeshModel(GeometryModel *, PolygonMode, const Vector3<float> &);

            AIManager *aiManager;
            Renderer3d *renderer3d;

            std::vector<urchin::GeometryModel *> navMeshModels;
            unsigned int loadedNavMeshId;
    };

}

#endif
