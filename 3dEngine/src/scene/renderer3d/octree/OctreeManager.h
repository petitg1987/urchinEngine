#ifndef URCHINENGINE_OCTREEMANAGER_H
#define URCHINENGINE_OCTREEMANAGER_H

#include <limits>
#include <unordered_set>
#include <stdexcept>
#include <vector>
#include "UrchinCommon.h"

#include "Octree.h"
#include "scene/renderer3d/octree/filter/OctreeableFilter.h"
#include "scene/renderer3d/octree/filter/AcceptAllFilter.h"
#include "utils/display/geometry/aabbox/AABBoxModel.h"
#include "utils/shader/ShaderManager.h"

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
			const std::unordered_set<TOctreeable *> &getMovingOctreeables() const;

			std::unordered_set<TOctreeable *> getAllOctreeables() const;
			void getOctreeablesIn(const ConvexObject3D<float> &, std::unordered_set<TOctreeable *> &) const;
			void getOctreeablesIn(const ConvexObject3D<float> &, std::unordered_set<TOctreeable *> &, const OctreeableFilter<TOctreeable> &) const;
		
			#ifdef _DEBUG
				void drawOctree(const Matrix4<float> &, const Matrix4<float> &) const;
			#endif
		
		private:
			void buildOctree(std::unordered_set<TOctreeable *> &);
			bool resizeOctree(TOctreeable *);
		
			float overflowSize;
			int minSize;
			Octree<TOctreeable> *mainOctree;

			std::unordered_set<TOctreeable *> movingOctreeables;
			mutable std::vector<Octree<TOctreeable> *> browseNodes;

			#ifdef _DEBUG
				unsigned int refreshModCount, postRefreshModCount;
			#endif
	};

	#include "OctreeManager.inl"

}

#endif
