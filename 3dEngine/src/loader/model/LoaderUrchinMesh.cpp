#include <locale>
#include <stdexcept>
#include <UrchinCommon.h>

#include <loader/model/LoaderUrchinMesh.h>

namespace urchin {

    ConstMeshes* LoaderUrchinMesh::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        std::locale::global(std::locale("C")); //for float

        std::istringstream iss;
        std::string buffer;
        std::string sdata;
        int idata = 0;

        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
        std::ifstream file(filenamePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        //numBones
        unsigned int numBones = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numBones;

        //numMeshes
        unsigned int numMeshes = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numMeshes;

        //bones
        std::vector<Bone> baseSkeleton(numBones);
        FileReader::nextLine(file, buffer); //buffer = "joints {"
        for (unsigned int i = 0; i < numBones; i++) {
            FileReader::nextLine(file, buffer);
            Bone* bone = &baseSkeleton[i];
            iss.clear(); iss.str(buffer);
            iss >> bone->name >> bone->parent >> sdata >> bone->pos.X >> bone->pos.Y >> bone->pos.Z >> sdata >> sdata >> bone->orient.X >> bone->orient.Y >> bone->orient.Z;
            bone->orient.computeW();
            bone->name = bone->name.substr(1, bone->name.length() - 2); //remove quote
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //mesh
        std::vector<const ConstMesh*> constMeshes;
        for (unsigned int ii = 0; ii < numMeshes; ii++) {
            //material
            std::string materialFilename;
            FileReader::nextLine(file, buffer); //buffer= "mesh {"
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> materialFilename;
            materialFilename = materialFilename.substr(1, materialFilename.length()-2); //remove quote

            //numVertices
            unsigned int numVertices = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numVertices;

            //vertices
            std::vector<Vertex> vertices(numVertices);
            std::vector<Point2<float>> textureCoordinates(numVertices);
            for (unsigned int i = 0; i < numVertices; i++) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                iss >> sdata >> idata >> vertices[i].linkedVerticesGroupId >> sdata >> textureCoordinates[i].X >> textureCoordinates[i].Y
                        >> sdata >> sdata >> vertices[i].weightStart >> vertices[i].weightCount >> sdata;
            }

            //numTriangles
            unsigned int numTriangles = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numTriangles;

            //triangles indices
            std::vector<unsigned int> trianglesIndices(numTriangles * 3);
            for (unsigned int i = 0, triVertexIndex = 0; i < numTriangles; i++, triVertexIndex += 3) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                iss >> sdata >> idata >> trianglesIndices[triVertexIndex] >> trianglesIndices[triVertexIndex + 1] >> trianglesIndices[triVertexIndex + 2];
            }

            //numWeights
            unsigned int numWeights = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numWeights;

            //weights
            std::vector<Weight> weights(numWeights);
            for (unsigned int i = 0; i < numWeights; i++) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                iss >> sdata >> idata >> weights[i].bone >> weights[i].bias >> sdata >> weights[i].pos.X >> weights[i].pos.Y >> weights[i].pos.Z;
            }
            FileReader::nextLine(file, buffer); //buffer= "}"

            constMeshes.push_back(new ConstMesh(materialFilename, vertices, textureCoordinates, trianglesIndices, weights, baseSkeleton));
        }

        file.close();

        return new ConstMeshes(filename, constMeshes);
    }

}
