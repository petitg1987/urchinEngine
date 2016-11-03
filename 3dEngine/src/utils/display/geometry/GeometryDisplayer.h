#ifndef ENGINE_GEOMETRYDISPLAYER_H
#define ENGINE_GEOMETRYDISPLAYER_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin
{

	class GeometryDisplayer
	{
		public:
			GeometryDisplayer();
			virtual ~GeometryDisplayer();

			void addGeometry(GeometryModel *);
			void removeGeometry(GeometryModel *);
			void removeAllGeometries();

			void onCameraProjectionUpdate(const Camera *const);

			void display(const Matrix4<float> &viewMatrix) const;

		private:
			std::vector<GeometryModel *> geometryModels;
			Matrix4<float> projectionMatrix;
	};

}

#endif
