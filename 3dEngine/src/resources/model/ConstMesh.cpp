#include <GL/glew.h>
#include <stdexcept>
#include <limits>

#include "resources/model/ConstMesh.h"
#include "resources/model/MeshService.h"
#include "resources/MediaManager.h"

namespace urchin
{

	ConstMesh::ConstMesh(const std::string &materialFilename, const std::vector<Vertex> &vertices, const std::vector<TextureCoordinate> &textureCoordinates,
			const std::vector<Triangle> &triangles, const std::vector<Weight> &weights, const std::vector<Bone> &baseSkeleton) :
		vertices(vertices),
		textureCoordinates(textureCoordinates),
		triangles(triangles),
		weights(weights),
		baseSkeleton(baseSkeleton),
		baseVertices(new Point3<float>[vertices.size()]),
		baseDataVertices(new DataVertex[vertices.size()])
	{
		//regroup duplicate vertex due to their different texture coordinates
		for(unsigned int i=0;i<vertices.size();++i)
		{
			linkedVertices[vertices[i].linkedVerticesGroupId].push_back(i);
		}

		//compute vertices and normals based on bind-pose skeleton
		MeshService::instance()->computeVertices(this, baseSkeleton, baseVertices);
		MeshService::instance()->computeNormals(this, baseVertices, baseDataVertices);

		//load material
		material = MediaManager::instance()->getMedia<Material>(materialFilename);
	}

	ConstMesh::~ConstMesh()
	{
		delete [] baseVertices;
		delete [] baseDataVertices;

		material->release();
	}

	const Material *ConstMesh::getMaterial() const
	{
		return material;
	}

	unsigned int ConstMesh::getNumberVertices() const
	{
		return vertices.size();
	}

	const Vertex &ConstMesh::getStructVertex(unsigned int index) const
	{
		return vertices[index];
	}

	const std::vector<TextureCoordinate> &ConstMesh::getTextureCoordinates() const
	{
		return textureCoordinates;
	}

	/**
	 * Vertices can be duplicated because they have different texture coordinates.
	 * This method returns all duplicates vertices thanks to 'linked vertices group ID' stored on each vertex.
	 */
	const std::vector<unsigned int> &ConstMesh::getLinkedVertices(unsigned int linkedVerticesGroupId) const
	{
		auto it = linkedVertices.find(linkedVerticesGroupId);
		if(it!=linkedVertices.end())
		{
			return it->second;
		}

		throw std::runtime_error("Impossible to find linked vertices for group ID: " + std::to_string(linkedVerticesGroupId));
	}

	unsigned int ConstMesh::getNumberTriangles() const
	{
		return triangles.size();
	}

	const std::vector<Triangle> &ConstMesh::getTriangles() const
	{
		return triangles;
	}

	const Triangle &ConstMesh::getTriangle(unsigned int index) const
	{
		return triangles[index];
	}

	unsigned int ConstMesh::getNumberWeights() const
	{
		return weights.size();
	}

	const Weight &ConstMesh::getWeight(unsigned int index) const
	{
		return weights[index];
	}

	unsigned int ConstMesh::getNumberBones() const
	{
		return baseSkeleton.size();
	}

	const std::vector<Bone> &ConstMesh::getBaseSkeleton() const
	{
		return baseSkeleton;
	}

	const Bone &ConstMesh::getBaseBone(unsigned int index) const
	{
		return baseSkeleton[index];
	}

	const Point3<float> *ConstMesh::getBaseVertices() const
	{
		return baseVertices;
	}

	const DataVertex *ConstMesh::getBaseDataVertices() const
	{
		return baseDataVertices;
	}

}
