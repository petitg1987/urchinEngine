#ifndef ENGINE_OCTREEMANAGER_H
#define ENGINE_OCTREEMANAGER_H

#include <GL/gl.h>
#include <limits>
#include <set>
#include <stdexcept>
#include "UrchinCommon.h"

#include "Octree.h"
#include "scene/renderer3d/octree/filter/OctreeableFilter.h"
#include "scene/renderer3d/octree/filter/AcceptAllFilter.h"
#include "utils/display/geometry/aabbox/AABBoxModel.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{
	
	template<class TOctreeable> class OctreeManager : public Observable
	{
		public:
			OctreeManager(int);
			~OctreeManager();

			enum NotificationType
			{
				OCTREE_BUILT,
			};

			void initialize();

			void setDepth(int);
		
			void addOctreeable(TOctreeable *);
			void removeOctreeable(TOctreeable *);
			std::set<TOctreeable *> getOctreeables() const;

			const Octree<TOctreeable> &getMainOctree() const;
			std::set<TOctreeable *> getOctreeablesIn(const ConvexObject3D<float> &);
			std::set<TOctreeable *> getOctreeablesIn(const ConvexObject3D<float> &, const OctreeableFilter<TOctreeable> &);
		
			#ifdef _DEBUG
				void drawOctree(const Matrix4<float> &, const Matrix4<float> &) const;
			#endif
		
		private:
			void refreshOctreeables();

			void buildOctree(std::set<TOctreeable *> &);
			bool resizeOctree(TOctreeable *);
		
			bool isInitialized;
			float overflowSize;
			int depth;
			Octree<TOctreeable> *mainOctree;
	};

	#include "OctreeManager.inl"

}

#endif
