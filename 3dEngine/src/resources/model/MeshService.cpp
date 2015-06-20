#include "resources/model/MeshService.h"
#include "resources/model/ConstAnimation.h"
#include "resources/model/ConstMeshes.h"

namespace urchin
{

	MeshService::MeshService() : Singleton<MeshService>()
	{

	}

	MeshService::~MeshService()
	{

	}

	void MeshService::computeVertices(const ConstMesh *const constMesh, const Bone *const skeleton, Point3<float> *const vertices)
	{
		//setup vertices
		for(unsigned int i=0;i<constMesh->getNumberVertices();++i)
		{
			vertices[i].setNull();

			//calculate final vertex to draw with weights
			for(int j=0;j<constMesh->getStructVertex(i).count;++j)
			{
				const Weight *weight = &constMesh->getWeight(constMesh->getStructVertex(i).start + j);

				//calculate transformed vertex for this weight
				Point3<float> wv = skeleton[weight->bone].orient.rotatePoint(weight->pos);

				//the sum of all weight->bias should be 1.0
				vertices[i].X += (skeleton[weight->bone].pos.X + wv.X) * weight->bias;
				vertices[i].Y += (skeleton[weight->bone].pos.Y + wv.Y) * weight->bias;
				vertices[i].Z += (skeleton[weight->bone].pos.Z + wv.Z) * weight->bias;
			}
		}
	}

	void MeshService::computeNormals(const ConstMesh *const constMesh, const Point3<float> *const vertices, DataVertex *const dataVertices)
	{
		//1. compute the normals for each triangle
		std::vector<Vector3<float>> normalByTriangles;
		for(unsigned int i=0;i<constMesh->getNumberTriangles();++i)
		{
			const int *pTris = constMesh->getTriangle(i).index; //shortcut

			//create two vectors to calculate the cross product
			const Vector3<float> &v1 = vertices[pTris[0]].vector(vertices[pTris[1]]).normalize();
			const Vector3<float> &v2 = vertices[pTris[2]].vector(vertices[pTris[0]]).normalize();

			normalByTriangles.push_back(v1.crossProduct(v2).normalize());
		}

		//2. compute the normals for each vertex
		for(unsigned int i=0;i<constMesh->getNumberVertices();i++)
		{
			dataVertices[i].normal.setNull();
			dataVertices[i].nbFace = 0;
		}
		for(unsigned int i=0;i<constMesh->getNumberTriangles();++i)
		{
			const int *pTris = constMesh->getTriangle(i).index; //shortcut

			for(int j=0;j<3;j++)
			{
				//adds the normal to the previous normal (average between normals)
				float nbFace = (float)dataVertices[pTris[j]].nbFace + 1.0;
				dataVertices[pTris[j]].normal = ((normalByTriangles[i] + dataVertices[pTris[j]].normal * (float)dataVertices[pTris[j]].nbFace) / nbFace).normalize();
				dataVertices[pTris[j]].nbFace++;

				//computes tangent
				const Vector3<float> &c1 = dataVertices[pTris[j]].normal.crossProduct(Vector3<float>(0.0, 0.0, 1.0));
				const Vector3<float> &c2 = dataVertices[pTris[j]].normal.crossProduct(Vector3<float>(0.0, 1.0, 0.0));
				if(c1.squareLength() > c2.squareLength())
				{
					dataVertices[pTris[j]].tangent = c1.normalize();
				}else
				{
					dataVertices[pTris[j]].tangent = c2.normalize();
				}
			}
		}
	}
}
