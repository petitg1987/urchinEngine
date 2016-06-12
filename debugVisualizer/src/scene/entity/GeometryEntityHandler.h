#ifndef ENGINE_DEBUGVISUALIZER_GEOMETRYENTITYHANDLER_H
#define ENGINE_DEBUGVISUALIZER_GEOMETRYENTITYHANDLER_H

#include <list>
#include <string>
#include "scene/entity/GeometryEntity.h"

namespace urchin
{

	class GeometryEntityHandler
	{
		public:
			GeometryEntityHandler();
			~GeometryEntityHandler();

			void add(GeometryEntity *);
			GeometryEntity *remove(const std::string &);
			GeometryEntity *remove(GeometryEntity *);
			void removeAll();

			const GeometryEntity *getGeometryEntity(const std::string &) const;
			const std::list<GeometryEntity *> &getGeometryEntities() const;

		private:
			std::list<GeometryEntity *> geometryEntities;
	};

}

#endif
