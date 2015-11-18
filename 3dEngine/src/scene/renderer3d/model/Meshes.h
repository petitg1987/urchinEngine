#ifndef ENGINE_MESHES_H
#define ENGINE_MESHES_H

#include <vector>
#include "UrchinCommon.h"

#include "resources/model/ConstMeshes.h"
#include "Mesh.h"

namespace urchin
{

	class Meshes
	{
		public:
			Meshes(const ConstMeshes *);
			~Meshes();
		
			unsigned int getNumberMeshes() const;
			Mesh *getMesh(unsigned int);
			const AABBox<float> &getGlobalAABBox() const;
			const std::vector<AABBox<float>> &getGlobalSplittedAABBox() const;
			const AABBox<float> &getGlobalLocalAABBox() const;

			void onMoving(const Transform<float> &);
		
		private:
			const ConstMeshes *const constMeshes;
			unsigned int numMeshes;
		
			std::vector<Mesh *> meshes;
			AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
			std::vector<AABBox<float>> globalSplittedBBox;
	};

}

#endif
