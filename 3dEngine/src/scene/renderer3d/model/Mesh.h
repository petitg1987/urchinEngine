#ifndef URCHINENGINE_MESH_H
#define URCHINENGINE_MESH_H

#include "UrchinCommon.h"

#include "resources/model/ConstMesh.h"
#include "scene/renderer3d/model/displayer/MeshParameter.h"

namespace urchin
{

	class Mesh
	{
		public:
			explicit Mesh(const ConstMesh *);
			~Mesh();

			void update(const std::vector<Bone> &);

			void display(const MeshParameter &) const;

		private:
			const ConstMesh *const constMesh;

			Point3<float> *const vertices;
			DataVertex *const dataVertices; //additional information for the vertex

			unsigned int bufferIDs[4], vertexArrayObject;
			enum //buffer IDs indices
			{
				VAO_VERTEX_POSITION = 0,
				VAO_TEX_COORD,
				VAO_NORMAL_TANGENT,
				VAO_INDEX
			};
			enum //shader input
			{
				SHADER_VERTEX_POSITION = 0,
				SHADER_TEX_COORD,
				SHADER_NORMAL,
				SHADER_TANGENT
			};

	};

}

#endif
