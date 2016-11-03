#include <algorithm>

#include "utils/display/geometry/GeometryDisplayer.h"

namespace urchin
{

	GeometryDisplayer::GeometryDisplayer()
	{

	}

	GeometryDisplayer::~GeometryDisplayer()
	{

	}

	void GeometryDisplayer::addGeometry(GeometryModel *geometry)
	{
		geometryModels.push_back(geometry);

		geometry->onCameraProjectionUpdate(projectionMatrix);
	}

	void GeometryDisplayer::removeGeometry(GeometryModel *geometry)
	{
		std::vector<GeometryModel *>::iterator it = std::find(geometryModels.begin(), geometryModels.end(), geometry);
		if(it!=geometryModels.end())
		{
			geometryModels.erase(it);
		}
	}

	void GeometryDisplayer::removeAllGeometries()
	{
		geometryModels.clear();
	}

	void GeometryDisplayer::onCameraProjectionUpdate(const Camera *const camera)
	{
		this->projectionMatrix = camera->getProjectionMatrix();

		for(std::vector<GeometryModel *>::iterator it = geometryModels.begin(); it!=geometryModels.end(); ++it)
		{
			(*it)->onCameraProjectionUpdate(projectionMatrix);
		}
	}

	void GeometryDisplayer::display(const Matrix4<float> &viewMatrix) const
	{
		for(std::vector<GeometryModel *>::const_iterator it = geometryModels.begin(); it!=geometryModels.end(); ++it)
		{
			(*it)->display(viewMatrix);
		}
	}

}
