#ifndef ENGINE_DEBUGVISUALIZER_GEOMETRYENTITY_H
#define ENGINE_DEBUGVISUALIZER_GEOMETRYENTITY_H

#include <string>
#include <memory>
#include "Urchin3dEngine.h"

namespace urchin
{

	class GeometryEntity
	{
		public:
			GeometryEntity(const std::string &, std::shared_ptr<GeometryModel>);
			~GeometryEntity();

			const std::string getId() const;
			std::shared_ptr<GeometryModel> getGeometryModel() const;

		private:
			std::string id;
			std::shared_ptr<GeometryModel> geometryModel;
	};

}

#endif
