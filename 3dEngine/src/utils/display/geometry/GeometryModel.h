#ifndef ENGINE_GEOMETRYMODEL_H
#define ENGINE_GEOMETRYMODEL_H

#include "UrchinCommon.h"

namespace urchin
{

	class GeometryModel
	{
		public:
			GeometryModel();
			virtual ~GeometryModel();

			enum PolygonMode
			{
				WIREFRAME,
				FILL
			};

			void onCameraProjectionUpdate(const Matrix4<float> &);

			Vector4<float> getColor() const;
			void setColor(float, float, float);
			void setAlpha(float);

			PolygonMode getPolygonMode() const;
			void setPolygonMode(PolygonMode);

			void display(const Matrix4<float> &) const;

		protected:
			virtual Matrix4<float> retrieveModelMatrix() const = 0;
			virtual std::vector<Point3<float>> retrieveVertexArray() const = 0;

			void initialize();

			virtual void drawGeometry() const = 0;

		private:
			unsigned int bufferIDs[1], vertexArrayObject;
			enum //buffer IDs indexes
			{
				VAO_VERTEX_POSITION = 0,
			};
			enum //shader input
			{
				SHADER_VERTEX_POSITION = 0,
			};
			unsigned int shader;
			int mProjectionLoc, mViewLoc, colorLoc;

			Matrix4<float> projectionMatrix;
			float red, green, blue, alpha;
			PolygonMode polygonMode;

			Matrix4<float> modelMatrix;
	};

}

#endif
