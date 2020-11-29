#ifndef URCHINENGINE_NAVOBJECT_H
#define URCHINENGINE_NAVOBJECT_H

#include <memory>
#include <vector>
#include <map>

#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/model/output/NavPolygon.h"

namespace urchin {

    class NavObject {
        public:
            explicit NavObject(std::shared_ptr<Polytope>);

            const std::shared_ptr<Polytope> &getExpandedPolytope();

            void addWalkableSurface(const std::shared_ptr<PolytopeSurface>&);
            const std::vector<std::shared_ptr<PolytopeSurface>> &getWalkableSurfaces() const;

            void addNearObject(const std::weak_ptr<NavObject>&);
            const std::vector<std::weak_ptr<NavObject>> &retrieveNearObjects();
            void removeAllNearObjects();

            void addNavPolygons(const std::vector<std::shared_ptr<NavPolygon>>&);
            const std::vector<std::shared_ptr<NavPolygon>> &getNavPolygons() const;
            void removeAllNavPolygons();

        private:
            std::shared_ptr<Polytope> expandedPolytope;
            std::vector<std::shared_ptr<PolytopeSurface>> walkableSurfaces;
            std::vector<std::weak_ptr<NavObject>> nearObjects; //use weak_ptr to avoid cyclic references (=memory leak) between navigation object
            std::vector<std::shared_ptr<NavPolygon>> navPolygons;
    };

}

#endif
