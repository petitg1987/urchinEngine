#include <algorithm>

#include "utils/display/geometry/GeometryDisplayer.h"

namespace urchin
{

	void GeometryDisplayer::addGeometry(GeometryModel *geometry)
	{
		geometryModels.push_back(geometry);

		geometry->onCameraProjectionUpdate(projectionMatrix);
	}

	void GeometryDisplayer::removeGeometry(GeometryModel *geometry)
	{
		auto it = std::find(geometryModels.begin(), geometryModels.end(), geometry);
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

		for (auto &geometryModel : geometryModels)
		{
			geometryModel->onCameraProjectionUpdate(projectionMatrix);
		}
	}

	void GeometryDisplayer::display(const Matrix4<float> &viewMatrix) const
	{
		for (auto geometryModel : geometryModels)
		{
			geometryModel->display(viewMatrix);
		}
	}

}
