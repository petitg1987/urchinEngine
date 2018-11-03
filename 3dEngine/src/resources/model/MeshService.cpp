#include <limits>
#include <chrono>

#include "resources/model/MeshService.h"
#include "resources/model/ConstAnimation.h"

namespace urchin
{

	MeshService::MeshService() : Singleton<MeshService>()
	{

	}

	void MeshService::computeVertices(const ConstMesh *const constMesh, const std::vector<Bone> &skeleton, Point3<float> *const vertices)
	{
		//setup vertices
		for(unsigned int i=0;i<constMesh->getNumberVertices();++i)
		{
			vertices[i].setNull();

			//calculate final vertex to draw with weights
			for(int j=0;j<constMesh->getStructVertex(i).weightCount;++j)
			{
				const Weight *weight = &constMesh->getWeight(static_cast<unsigned int>(constMesh->getStructVertex(i).weightStart + j));
				const Bone &bone = skeleton[weight->bone];

				//calculate transformed vertex for this weight
				Point3<float> wv = bone.orient.rotatePoint(weight->pos);

				//the sum of all weight->bias should be 1.0
				vertices[i].X += (bone.pos.X + wv.X) * weight->bias;
				vertices[i].Y += (bone.pos.Y + wv.Y) * weight->bias;
				vertices[i].Z += (bone.pos.Z + wv.Z) * weight->bias;
			}
		}
	}

	void MeshService::computeNormals(const ConstMesh *const constMesh, const Point3<float> *const vertices, DataVertex *const dataVertices)
    {
        //compute weighted normals
        std::vector<Vector3<float>> vertexNormals(constMesh->getNumberVertices(), Vector3<float>(0.0f, 0.0f, 0.0f));
        for(unsigned int triIndex=0; triIndex<constMesh->getNumberTriangles(); ++triIndex)
        {
            const Triangle &tri = constMesh->getTriangle(triIndex);
            for(unsigned int triVertexIndex=0; triVertexIndex<3; ++triVertexIndex)
            {
                auto vertexIndex = static_cast<unsigned int>(tri.index[triVertexIndex]);

                Vector3<float> weightedNormal = computeWeightedVertexNormal(tri, triVertexIndex, vertices);
                vertexNormals[vertexIndex] += weightedNormal;
            }
        }

        //sum weighted normals of same vertex
        for (unsigned int vertexIndex = 0; vertexIndex < constMesh->getNumberVertices(); ++vertexIndex)
        {
            unsigned int linkedVerticesGroupId = constMesh->getStructVertex(vertexIndex).linkedVerticesGroupId;
            std::vector<unsigned int> linkedVertices = constMesh->getLinkedVertices(linkedVerticesGroupId);
            assert(!linkedVertices.empty()); //contains at least 'vertexIndex'

            for(unsigned int linkedVertex : linkedVertices)
            {
                dataVertices[vertexIndex].normal += vertexNormals[linkedVertex];
            }
        }

        for (unsigned int vertexIndex = 0; vertexIndex < constMesh->getNumberVertices(); ++vertexIndex)
        {
            //normal normalization
            dataVertices[vertexIndex].normal = dataVertices[vertexIndex].normal.normalize();

            //computes tangent
            const Vector3<float> &c1 = dataVertices[vertexIndex].normal.crossProduct(Vector3<float>(0.0f, 0.0f, 1.0f));
            const Vector3<float> &c2 = dataVertices[vertexIndex].normal.crossProduct(Vector3<float>(0.0f, 1.0f, 0.0f));
            if(c1.squareLength() > c2.squareLength())
            {
                dataVertices[vertexIndex].tangent = c1.normalize();
            }else
            {
                dataVertices[vertexIndex].tangent = c2.normalize();
            }
        }
	}

    int MeshService::indexOfVertexInTriangle(const Triangle &triangle, unsigned int vertexIndex, const ConstMesh *const constMesh)
    {
        for(unsigned int i=0; i<3; ++i)
        {
            auto triVertexIndex = static_cast<unsigned int>(triangle.index[i]);

            //classic
            if(triVertexIndex==vertexIndex)
            {
                return i;
            }

            //by group id
            unsigned int linkedVerticesGroupId = constMesh->getStructVertex(triVertexIndex).linkedVerticesGroupId;
            std::vector<unsigned int> linkedVertices = constMesh->getLinkedVertices(linkedVerticesGroupId);
            for (unsigned int linkedVertexIndex : linkedVertices)
            {
                if (linkedVertexIndex == vertexIndex)
                {
                    return i;
                }
            }
        }

        return -1;
    }

    Vector3<float> MeshService::computeWeightedVertexNormal(const Triangle &triangle, unsigned int vertexIndex, const Point3<float> *const vertices)
    { //see https://stackoverflow.com/questions/18519586/calculate-normal-per-vertex-opengl
        Point3<float> a = vertices[triangle.index[vertexIndex]];
        Point3<float> b = vertices[triangle.index[(vertexIndex+1)%3]];
        Point3<float> c = vertices[triangle.index[(vertexIndex+2)%3]];

        Vector3<float> ab = a.vector(b);
        Vector3<float> ac = a.vector(c);

        Vector3<float> normal = ac.crossProduct(ab);
        float sinAlpha = normal.length() / (ab.length() * ac.length());
        sinAlpha = MathAlgorithm::clamp(sinAlpha, -1.0f + std::numeric_limits<float>::epsilon(), 1.0f - std::numeric_limits<float>::epsilon()); //because of rounding error

        return normal.normalize() * std::asin(sinAlpha);
    }
}
