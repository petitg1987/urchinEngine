#include "GeometryEntity.h"

namespace urchin
{

	GeometryEntity::GeometryEntity(const std::string &id, std::shared_ptr<GeometryModel> geometryModel) :
			id(id),
			geometryModel(geometryModel)
	{

	}

	GeometryEntity::~GeometryEntity()
	{

	}

	const std::string GeometryEntity::getId() const
	{
		return id;
	}

	std::shared_ptr<GeometryModel> GeometryEntity::getGeometryModel() const
	{
		return geometryModel;
	}

}
