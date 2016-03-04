#ifndef ENGINE_CONSTMESH_H
#define ENGINE_CONSTMESH_H

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
		int nbFace; //numbers of face for the vertex
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
			ConstMesh(const std::string &, unsigned int, const Vertex *const, const TextureCoordinate *const,
					unsigned int, const Triangle *const, unsigned int, const Weight *const, unsigned int,
					const Bone *const, void *);
			~ConstMesh();

			const Material *getMaterial() const;

			unsigned int getNumberVertices() const;
			const Vertex &getStructVertex(unsigned int) const;
			const TextureCoordinate *getTextureCoordinates() const;
			std::vector<unsigned int> getLinkedVertices(unsigned int) const;

			unsigned int getNumberTriangles() const;
			const Triangle *getTriangles() const;
			const Triangle &getTriangle(unsigned int) const;

			unsigned int getNumberWeights() const;
			const Weight &getWeight(unsigned int) const;

			unsigned int getNumberBones() const;
			const Bone *getBaseSkeleton() const;
			const Bone &getBaseBone(unsigned int) const;
			const Point3<float> *getBaseVertices() const;
			const DataVertex *getBaseDataVertices() const;

		private:
			void defineTextureWrap();

			Material *material;

			const unsigned int numVertices;
			const Vertex *const vertices;
			const TextureCoordinate *const textureCoordinates;
			std::map<unsigned int, std::vector<unsigned int>> linkedVertices;

			const unsigned int numTriangles;
			const Triangle *const triangles;

			const unsigned int numWeights;
			const Weight *const weights;

			//mesh information in bind-pose
			const unsigned int numBones;
			const Bone *const baseSkeleton; //bind-pose skeleton
			Point3<float> *const baseVertices;
			DataVertex *const baseDataVertices; //additional informations for the vertex
	};
}

#endif
