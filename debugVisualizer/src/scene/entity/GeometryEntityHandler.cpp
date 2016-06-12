#include <stdexcept>

#include "GeometryEntityHandler.h"

namespace urchin
{

	GeometryEntityHandler::GeometryEntityHandler()
	{

	}

	GeometryEntityHandler::~GeometryEntityHandler()
	{

	}

	void GeometryEntityHandler::add(GeometryEntity *geometryEntity)
	{
		geometryEntities.push_back(geometryEntity);
	}

	GeometryEntity *GeometryEntityHandler::remove(const std::string &id)
	{
		std::list<GeometryEntity *>::iterator it;
		for(it=geometryEntities.begin(); it!=geometryEntities.end(); ++it)
		{
			if((*it)->getId().compare(id)==0)
			{
				geometryEntities.erase(it);
				break;
			}
		}

		if(it==geometryEntities.end())
		{
			throw std::runtime_error("Impossible to find geometry entity to remove with id: " + id);
		}

		return (*it);
	}

	GeometryEntity *GeometryEntityHandler::remove(GeometryEntity *geometryEntity)
	{
		geometryEntities.remove(geometryEntity);

		return geometryEntity;
	}

	void GeometryEntityHandler::removeAll()
	{
		geometryEntities.clear();
	}

	const GeometryEntity *GeometryEntityHandler::getGeometryEntity(const std::string &id) const
	{
		for(std::list<GeometryEntity *>::const_iterator it=geometryEntities.begin(); it!=geometryEntities.end(); ++it)
		{
			if((*it)->getId().compare(id)==0)
			{
				return *it;
			}
		}

		throw std::runtime_error("Impossible to find geometry entity with id: " + id);
	}

	const std::list<GeometryEntity *> &GeometryEntityHandler::getGeometryEntities() const
	{
		return geometryEntities;
	}

}
