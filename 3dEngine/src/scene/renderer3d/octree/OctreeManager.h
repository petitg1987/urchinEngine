#ifndef URCHINENGINE_OCTREEMANAGER_H
#define URCHINENGINE_OCTREEMANAGER_H

#include <limits>
#include <stdexcept>
#include <vector>
#include "UrchinCommon.h"

#include "Octree.h"
#include "scene/renderer3d/octree/filter/OctreeableFilter.h"
#include "scene/renderer3d/octree/filter/AcceptAllFilter.h"

namespace urchin
{
	
	template<class TOctreeable> class OctreeManager : public Observable, public Observer
	{
		public:
			explicit OctreeManager(float);
			~OctreeManager() override;

			enum NotificationType
			{
				OCTREE_BUILT,
			};

			void notify(Observable *, int) override;
		
			void addOctreeable(TOctreeable *);
			void removeOctreeable(TOctreeable *);

			void updateMinSize(float);
			void refreshOctreeables();
			void postRefreshOctreeables();

			const Octree<TOctreeable> &getMainOctree() const;
			std::vector<const Octree<TOctreeable> *> getAllLeafOctrees() const;

			std::vector<TOctreeable *> getAllOctreeables() const;
			void getOctreeablesIn(const ConvexObject3D<float> &, std::vector<TOctreeable *> &) const;
			void getOctreeablesIn(const ConvexObject3D<float> &, std::vector<TOctreeable *> &, const OctreeableFilter<TOctreeable> &) const;

		private:
			void buildOctree(std::vector<TOctreeable *> &);
			bool resizeOctree(TOctreeable *);
		
			float overflowSize;
			int minSize;
			Octree<TOctreeable> *mainOctree;

			std::vector<TOctreeable *> movingOctreeables;
			mutable std::vector<Octree<TOctreeable> *> browseNodes;

			#ifdef _DEBUG
				unsigned int refreshModCount, postRefreshModCount;
			#endif
	};

	#include "OctreeManager.inl"

}

#endif
