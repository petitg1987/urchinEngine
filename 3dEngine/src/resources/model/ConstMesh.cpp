#include <GL/gl.h>

#include "resources/model/ConstMesh.h"
#include "resources/model/MeshService.h"
#include "resources/MediaManager.h"

namespace urchin
{

	ConstMesh::ConstMesh(const std::string &materialFilename, unsigned int numVertices, const Vertex *const vertices,
			const St *const st, unsigned int numTriangles, const Triangle *const triangles, unsigned int numWeights,
			const Weight *const weights, unsigned int numBones, const Bone *const baseSkeleton, void *loaderParams) :
		numVertices(numVertices),
		vertices(vertices),
		st(st),
		numTriangles(numTriangles),
		triangles(triangles),
		numWeights(numWeights),
		weights(weights),
		numBones(numBones),
		baseSkeleton(baseSkeleton),
		baseVertices(new Point3<float>[numVertices]),
		baseDataVertices(new DataVertex[numVertices])
	{
		//compute vertices and normals based on bind-pose skeleton
		MeshService::instance()->computeVertices(this, baseSkeleton, baseVertices);
		MeshService::instance()->computeNormals(this, baseVertices, baseDataVertices);

		//load the material
		material = MediaManager::instance()->getMedia<Material>(materialFilename, loaderParams);
	}

	ConstMesh::~ConstMesh()
	{
		delete [] vertices;
		delete [] st;
		delete [] triangles;
		delete [] weights;
		delete [] baseSkeleton;

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
		return numVertices;
	}

	const Vertex &ConstMesh::getStructVertex(unsigned int index) const
	{
		return vertices[index];
	}

	const St *ConstMesh::getTextureCoordinates() const
	{
		return st;
	}

	unsigned int ConstMesh::getNumberTriangles() const
	{
		return numTriangles;
	}

	const Triangle *ConstMesh::getTriangles() const
	{
		return triangles;
	}

	const Triangle &ConstMesh::getTriangle(unsigned int index) const
	{
		return triangles[index];
	}

	unsigned int ConstMesh::getNumberWeights() const
	{
		return numWeights;
	}

	const Weight &ConstMesh::getWeight(unsigned int index) const
	{
		return weights[index];
	}

	unsigned int ConstMesh::getNumberBones() const
	{
		return numBones;
	}

	const Bone *ConstMesh::getBaseSkeleton() const
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
