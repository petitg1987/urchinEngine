#ifndef ENGINE_DEBUGVISUALIZER_CONVEXHULL3DACTION_H
#define ENGINE_DEBUGVISUALIZER_CONVEXHULL3DACTION_H

#include <string>
#include "UrchinCommon.h"

#include "scene/controller/action/Action.h"
#include "scene/entity/GeometryEntityHandler.h"
#include "scene/entity/GeometryEntity.h"

namespace urchin
{

	class ConvexHull3DAction : public Action
	{
		public:
			ConvexHull3DAction(const std::string &, const ConvexHull3D<float> &);
			~ConvexHull3DAction();

			void execute(std::shared_ptr<GeometryEntityHandler>);
			void reverse(std::shared_ptr<GeometryEntityHandler>);

		private:
			GeometryEntity *entity;

	};

}

#endif
