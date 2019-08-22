#ifndef URCHINENGINE_POLYTOPE_H
#define URCHINENGINE_POLYTOPE_H

#include <vector>
#include <sstream>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/model/NavMeshAgent.h"

namespace urchin
{

	class Polytope
	{
		public:
			Polytope(std::string, std::vector<std::shared_ptr<PolytopeSurface>> &);

			const std::string &getName() const;

			const std::vector<std::shared_ptr<PolytopeSurface>> &getSurfaces() const;
			const std::shared_ptr<PolytopeSurface> &getSurface(unsigned int) const;

			const std::unique_ptr<Rectangle<float>> &getXZRectangle() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void setObstacleCandidate(bool);
			bool isObstacleCandidate() const;

		private:
			void buildXZRectangle();

			std::string name;
			std::vector<std::shared_ptr<PolytopeSurface>> surfaces;

			std::unique_ptr<Rectangle<float>> xzRectangle;

			bool walkableCandidate;
			bool obstacleCandidate;
	};

	std::ostream& operator <<(std::ostream &, const Polytope &);

}

#endif
