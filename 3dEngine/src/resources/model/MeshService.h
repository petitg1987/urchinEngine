#ifndef URCHINENGINE_MESHSERVICE_H
#define URCHINENGINE_MESHSERVICE_H

#include "UrchinCommon.h"

namespace urchin
{
	class ConstMesh;
	struct Bone;
	struct DataVertex;

	class MeshService : public Singleton<MeshService>
	{
		public:
			friend class Singleton<MeshService>;

			void computeVertices(const ConstMesh *const, const std::vector<Bone> &, Point3<float> *const);
			void computeNormals(const ConstMesh *const, const Point3<float> *const,  DataVertex *const);

		private:
			MeshService();
			virtual ~MeshService();
	};

}

#endif
