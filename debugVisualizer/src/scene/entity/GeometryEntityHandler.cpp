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

	void GeometryEntityHandler::remove(const std::string &id)
	{
		bool found = false;
		for(std::list<GeometryEntity *>::iterator it=geometryEntities.begin(); it!=geometryEntities.end(); ++it)
		{
			if((*it)->getId().compare(id)==0)
			{
				geometryEntities.erase(it);
				found = true;
				break;
			}
		}

		if(!found)
		{
			throw std::runtime_error("Impossible to find geometry entity to remove with id: " + id);
		}
	}

	void GeometryEntityHandler::remove(GeometryEntity *geometryEntity)
	{
		geometryEntities.remove(geometryEntity);
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
