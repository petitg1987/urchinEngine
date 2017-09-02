#ifndef URCHINENGINE_OCTREEABLE_H
#define URCHINENGINE_OCTREEABLE_H

#include <vector>
#include <stdexcept>
#include "UrchinCommon.h"

#include "utils/display/geometry/aabbox/AABBoxModel.h"
#include "Octree.h"

namespace urchin
{
	
	/**
	* Virtual class that can be inserted into an octree
	*/
	template<class TOctreeable> class Octreeable : public Observable
	{
		public:
			Octreeable();
			virtual ~Octreeable();

			enum NotificationType
			{
				MOVE
			};
		
			void notifyOctreeableMove();
			void onMoveDone();
			bool isMovingInOctree() const;

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

			bool bIsMovingInOctree;
			bool bIsVisible;
	};

	#include "Octreeable.inl"

}

#endif
