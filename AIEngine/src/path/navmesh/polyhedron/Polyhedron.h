#ifndef ENGINE_POLYHEDRON_H
#define ENGINE_POLYHEDRON_H

#include <vector>
#include "path/navmesh/polyhedron/Face.h"

namespace urchin
{

	class Polyhedron
	{
		public:
			Polyhedron(const std::vector<Face> &faces);

			const std::vector<Face> &getFaces() const;

		private:
			std::vector<Face> faces;

	};

}

#endif
