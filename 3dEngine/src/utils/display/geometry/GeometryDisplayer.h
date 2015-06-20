#ifndef ENGINE_GEOMETRYDISPLAYER_H
#define ENGINE_GEOMETRYDISPLAYER_H

#include <utils/display/geometry/GeometryModel.h>
#include <list>
#include "UrchinCommon.h"

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

			void onCameraProjectionUpdate(const Camera *const);

			void display(const Matrix4<float> &viewMatrix) const;

		private:
			std::list<GeometryModel *> geometryModels;
			Matrix4<float> projectionMatrix;
	};

}

#endif
