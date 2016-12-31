#include "Polyhedron.h"

namespace urchin
{

	/**
	 * @param faces Faces of the polyhedron. Faces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
	 */
	Polyhedron::Polyhedron(const std::vector<Face> &faces) :
			faces(faces)
	{

	}

	const std::vector<Face> &Polyhedron::getFaces() const
	{
		return faces;
	}

}
