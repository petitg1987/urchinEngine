#ifndef ENGINE_CONSTMESHES_H
#define ENGINE_CONSTMESHES_H

#include <vector>
#include <string>
#include "UrchinCommon.h"

#include "resources/model/ConstMesh.h"
#include "resources/Resource.h"

namespace urchin
{
	
	/**
	 * Contains all the constant/common data for meshes.
	 * Two identical models can use the instance of this class.
	 */
	class ConstMeshes : public Resource
	{
		public:
			ConstMeshes(const std::string &, const std::vector<const ConstMesh *> &);
			~ConstMeshes();
		
			const std::string &getMeshFilename() const;
			unsigned int getNumberConstMeshes() const;
			const ConstMesh *getConstMesh(unsigned int) const;

			const AABBox<float> &getOriginalAABBox() const;
			const std::vector<AABBox<float>> &getOriginalSplittedAABBox() const;
		
		private:
			std::string meshFilename;
			std::vector<const ConstMesh *> constMeshes;

			AABBox<float> *originalBBox; //bounding box (not transformed)
			std::vector<AABBox<float>> originalSplittedBBox;
	};

}

#endif
