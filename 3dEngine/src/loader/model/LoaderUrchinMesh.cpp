#include <locale>
#include <stdexcept>
#include <memory>
#include "UrchinCommon.h"

#include "loader/model/LoaderUrchinMesh.h"

namespace urchin
{

	ConstMeshes *LoaderUrchinMesh::loadFromFile(const std::string &filename, void *params)
	{	
		std::locale::global(std::locale("C")); //for float
		
		std::ifstream file;
		std::istringstream iss;
		std::string buffer;
		std::string sdata;
		int idata;

		std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
		file.open(filenamePath, std::ios::in);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + filenamePath + ".");
		}

		//numBones
		unsigned int numBones;
		FileReaderUtil::nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> numBones;
		
		//numMeshes
		unsigned int numMeshes;
		FileReaderUtil::nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> numMeshes;

		//bones
		std::vector<Bone> baseSkeleton(numBones);
		FileReaderUtil::nextLine(file, buffer); //buffer = "joints {"
		for(unsigned int i=0;i<numBones;i++)
		{
			FileReaderUtil::nextLine(file, buffer);
			Bone *bone = &baseSkeleton[i];
			iss.clear(); iss.str(buffer);
			iss >> bone->name >> bone->parent >> sdata >> bone->pos.X >> bone->pos.Y >> bone->pos.Z >> sdata >> sdata >> bone->orient.X >> bone->orient.Y >> bone->orient.Z;
			bone->orient.computeW();
			bone->name = bone->name.substr(1, bone->name.length()-2); //remove quot
		}
		FileReaderUtil::nextLine(file, buffer); //buffer = "}"

		//mesh
		std::vector<const ConstMesh *> constMeshes;
		for(unsigned int ii=0; ii<numMeshes; ii++)
		{
			//material
			std::string materialFilename;
			FileReaderUtil::nextLine(file, buffer); //buffer= "mesh {"
			FileReaderUtil::nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> sdata >> materialFilename;
			materialFilename = materialFilename.substr(1, materialFilename.length()-2); //remove quot
			
			//numVertices
			unsigned int numVertices;
			FileReaderUtil::nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> sdata >> numVertices;
			
			//vertices
			std::vector<Vertex> vertices(numVertices);
			std::vector<TextureCoordinate> textureCoordinates(numVertices);
			for(unsigned int i=0;i<numVertices;i++)
			{
				FileReaderUtil::nextLine(file, buffer);
				iss.clear(); iss.str(buffer);
				iss >> sdata >> idata >> vertices[i].linkedVerticesGroupId >> sdata >> textureCoordinates[i].s >> textureCoordinates[i].t
						>> sdata >> sdata >> vertices[i].weightStart >> vertices[i].weightCount >> sdata;
			}
			
			//numTriangles
			unsigned int numTriangles;
			FileReaderUtil::nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> sdata >> numTriangles;

			//triangles
			std::vector<Triangle> triangles(numTriangles);
			for(unsigned int i=0;i<numTriangles;i++)
			{
				FileReaderUtil::nextLine(file, buffer);
				iss.clear(); iss.str(buffer);
				iss >> sdata >> idata >> triangles[i].index[0] >> triangles[i].index[1] >> triangles[i].index[2];
			}
			
			//numWeights
			unsigned int numWeights;
			FileReaderUtil::nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> sdata >> numWeights;
			
			//weights
			std::vector<Weight> weights(numWeights);
			for(unsigned int i=0;i<numWeights;i++)
			{
				FileReaderUtil::nextLine(file, buffer);
				iss.clear(); iss.str(buffer);
				iss >> sdata >> idata >> weights[i].bone >> weights[i].bias >> sdata >> weights[i].pos.X >> weights[i].pos.Y >> weights[i].pos.Z;
			}
			FileReaderUtil::nextLine(file, buffer); //buffer= "}"
			
			constMeshes.push_back(new ConstMesh(materialFilename, vertices, textureCoordinates, triangles, weights, baseSkeleton, params));
		}
		
		file.close();
		
		return new ConstMeshes(filename, constMeshes);
	}

}
