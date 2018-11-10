#ifndef URCHINENGINE_MESHSERVICE_H
#define URCHINENGINE_MESHSERVICE_H

#include "UrchinCommon.h"

namespace urchin
{
	class ConstMesh;
	struct Bone;
	struct DataVertex;
	struct Triangle;

	class MeshService : public Singleton<MeshService>
	{
		public:
			friend class Singleton<MeshService>;

			void computeVertices(const ConstMesh *, const std::vector<Bone> &, Point3<float> *);
			void computeNormals(const ConstMesh *, const Point3<float> *,  DataVertex *);

		private:
			MeshService();
			~MeshService() override = default;

			int indexOfVertexInTriangle(const Triangle &, unsigned int, const ConstMesh *const);
			Vector3<float> computeWeightedVertexNormal(const Triangle &, unsigned int, const Point3<float> *const );
	};

}

#endif
