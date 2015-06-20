#ifndef ENGINE_OCTREEABLE_H
#define ENGINE_OCTREEABLE_H

#include <utils/display/geometry/aabbox/AABBoxModel.h>
#include <vector>
#include <stdexcept>
#include "UrchinCommon.h"

#include "Octree.h"

namespace urchin
{
	
	/**
	* Virtual class that can be inserted into an octree
	*/
	template<class TOctreeable> class Octreeable
	{
		public:
			Octreeable();
			virtual ~Octreeable();
		
			void setToUpdate(bool);
			bool hasNeedUpdate();

			void setVisible(bool);
			bool isVisible()const;
		
			const std::vector<Octree<TOctreeable> *> &getRefOctree() const;
			void addRefOctree(Octree<TOctreeable> *);
			void removeRefOctree(Octree<TOctreeable> *);

			virtual const AABBox<float> &getAABBox() const = 0;
			virtual const Transform<float> &getTransform() const = 0;

			#ifdef _DEBUG
				void drawBBox(const Matrix4<float> &, const Matrix4<float> &) const;
			#endif
		
		private:
			std::vector<Octree<TOctreeable> *> refOctree;

			bool bNeedUpdate; //indicates whether the octreeable is moving
			bool bIsVisible; //indicates whether the octreeablel is visible
	};

	#include "Octreeable.inl"

}

#endif
