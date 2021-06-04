#pragma once

#include <vector>
#include <sstream>
#include <memory>
#include <UrchinCommon.h>

#include <path/navmesh/polytope/PolytopeSurface.h>
#include <path/navmesh/model/output/NavMeshAgent.h>

namespace urchin {

    class Polytope {
        public:
            Polytope(std::string, std::vector<std::shared_ptr<PolytopeSurface>>&);

            const std::string& getName() const;

            const std::vector<std::shared_ptr<PolytopeSurface>>& getSurfaces() const;
            const std::shared_ptr<PolytopeSurface>& getSurface(std::size_t) const;

            const Rectangle<float>& getXZRectangle() const;
            const AABBox<float>& getAABBox() const;

            void setWalkableCandidate(bool);
            bool isWalkableCandidate() const;

            void setObstacleCandidate(bool);
            bool isObstacleCandidate() const;

        private:
            void buildXZRectangle();
            void buildAABBox();

            std::string name;
            std::vector<std::shared_ptr<PolytopeSurface>> surfaces;

            Rectangle<float> xzRectangle;
            AABBox<float> aabbox;

            bool walkableCandidate;
            bool obstacleCandidate;
    };

    std::ostream& operator <<(std::ostream&, const Polytope&);

}
