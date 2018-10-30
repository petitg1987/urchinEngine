#ifndef URCHINENGINE_CONSTMESH_H
#define URCHINENGINE_CONSTMESH_H

#include <string>
#include <vector>
#include <map>
#include "UrchinCommon.h"

#include "resources/material/Material.h"

namespace urchin
{

	struct Vertex
	{
		unsigned int linkedVerticesGroupId;
		int weightStart;
		int weightCount;
	};

	struct TextureCoordinate
	{
		float s, t;
	};

	struct Triangle
	{
		int index[3]; //indices vertices
	};

	struct Weight
	{
		int bone; //index of the bone
		float bias; //contribution of the vertex weight
		Point3<float> pos; //coordinates of the vertex weight
	};

	struct DataVertex
	{
		Vector3<float> normal; //vector normal for each vertices
		Vector3<float> tangent; //vector tangent for each vertices
	};

	struct Bone
	{
		std::string name;
		int parent;
		Point3<float> pos;
		Quaternion<float> orient;
	};

	/**
	 * Contains all the constant/common data for a mesh.
	 * Two identical models can use the instance of this class.
	 */
	class ConstMesh
	{
		public:
			ConstMesh(const std::string &, const std::vector<Vertex> &, const std::vector<TextureCoordinate> &,
					const std::vector<Triangle> &, const std::vector<Weight> &, const std::vector<Bone> &);
			~ConstMesh();

			const Material *getMaterial() const;

			unsigned int getNumberVertices() const;
			const Vertex &getStructVertex(unsigned int) const;
			const std::vector<TextureCoordinate> &getTextureCoordinates() const;
			std::vector<unsigned int> getLinkedVertices(unsigned int) const;

			unsigned int getNumberTriangles() const;
			const std::vector<Triangle> &getTriangles() const;
			const Triangle &getTriangle(unsigned int) const;

			unsigned int getNumberWeights() const;
			const Weight &getWeight(unsigned int) const;

			unsigned int getNumberBones() const;
			const std::vector<Bone> &getBaseSkeleton() const;
			const Bone &getBaseBone(unsigned int) const;
			const Point3<float> *getBaseVertices() const;
			const DataVertex *getBaseDataVertices() const;

		private:
			Material *material;

			std::vector<Vertex> vertices;
			std::vector<TextureCoordinate> textureCoordinates;
			std::map<unsigned int, std::vector<unsigned int>> linkedVertices;

			std::vector<Triangle> triangles;

			std::vector<Weight> weights;

			//mesh information in bind-pose
			std::vector<Bone> baseSkeleton; //bind-pose skeleton
			Point3<float> *const baseVertices;
			DataVertex *const baseDataVertices; //additional information for the vertex
	};
}

#endif
