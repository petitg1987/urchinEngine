#ifndef ENGINE_DEBUGVISUALIZER_POINT3ACTION_H
#define ENGINE_DEBUGVISUALIZER_POINT3ACTION_H

#include <string>
#include "UrchinCommon.h"

#include "scene/controller/action/Action.h"
#include "scene/entity/GeometryEntityHandler.h"
#include "scene/entity/GeometryEntity.h"

namespace urchin
{

	class Point3Action : public Action
	{
		public:
			Point3Action(const std::string &, const Point3<float> &);
			~Point3Action();

			void execute(std::shared_ptr<GeometryEntityHandler>);
			void reverse(std::shared_ptr<GeometryEntityHandler>);

		private:
			GeometryEntity *entity;

	};

}

#endif
