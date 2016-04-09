#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "resources/model/ConstMesh.h"
#include "resources/model/MeshService.h"
#include "resources/MediaManager.h"

namespace urchin
{

	ConstMesh::ConstMesh(const std::string &materialFilename, unsigned int numVertices, const Vertex *const vertices,
			const TextureCoordinate *const textureCoordinates, unsigned int numTriangles, const Triangle *const triangles, unsigned int numWeights,
			const Weight *const weights, unsigned int numBones, const Bone *const baseSkeleton, void *loaderParams) :
		numVertices(numVertices),
		vertices(vertices),
		textureCoordinates(textureCoordinates),
		numTriangles(numTriangles),
		triangles(triangles),
		numWeights(numWeights),
		weights(weights),
		numBones(numBones),
		baseSkeleton(baseSkeleton),
		baseVertices(new Point3<float>[numVertices]),
		baseDataVertices(new DataVertex[numVertices])
	{
		//regroup duplicate vertex due to their different texture coordinates
		for(unsigned int i=0;i<numVertices;++i)
		{
			linkedVertices[vertices[i].linkedVerticesGroupId].push_back(i);
		}

		//compute vertices and normals based on bind-pose skeleton
		MeshService::instance()->computeVertices(this, baseSkeleton, baseVertices);
		MeshService::instance()->computeNormals(this, baseVertices, baseDataVertices);

		//load material
		material = MediaManager::instance()->getMedia<Material>(materialFilename, loaderParams);
		defineTextureWrap();
	}

	ConstMesh::~ConstMesh()
	{
		delete [] vertices;
		delete [] textureCoordinates;
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

	const TextureCoordinate *ConstMesh::getTextureCoordinates() const
	{
		return textureCoordinates;
	}

	/**
	 * Vertices can be duplicated because they have different texture coordinates.
	 * This method returns all duplicates vertices thanks to 'linked vertices group ID' stored on each vertex.
	 */
	std::vector<unsigned int> ConstMesh::getLinkedVertices(unsigned int linkedVerticesGroupId) const
	{
		std::map<unsigned int, std::vector<unsigned int>>::const_iterator it = linkedVertices.find(linkedVerticesGroupId);
		if(it!=linkedVertices.end())
		{
			return it->second;
		}

		throw std::runtime_error("Impossible to find linked vertices for group ID: " + linkedVerticesGroupId);
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

	void ConstMesh::defineTextureWrap()
	{ //GL_CLAMP_TO_EDGE should be used when it's possible: give better result on edge.
		bool needRepeatTexture = false;
		for(unsigned int i=0; i<numVertices; ++i)
		{
			if(textureCoordinates[i].s > 1.0f || textureCoordinates[i].s < 0.0f
					|| textureCoordinates[i].t > 1.0f || textureCoordinates[i].t < 0.0f)
			{
				needRepeatTexture = true;
				break;
			}
		}

		std::vector<const Image *> textures = material->getTextures();
		for(auto texture : textures)
		{
			glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, needRepeatTexture ? GL_REPEAT : GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, needRepeatTexture ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		}
	}

}
