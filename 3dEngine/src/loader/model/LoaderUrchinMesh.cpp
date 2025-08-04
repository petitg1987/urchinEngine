#include <locale>
#include <stdexcept>
#include <UrchinCommon.h>

#include "loader/model/LoaderUrchinMesh.h"
#include "resources/ResourceRetriever.h"
#include "resources/material/MaterialBuilder.h"

namespace urchin {

    std::shared_ptr<ConstMeshes> LoaderUrchinMesh::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        std::istringstream iss;
        iss.imbue(std::locale::classic());
        std::string buffer;
        std::string sdata;

        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filename);
        }

        //numBones
        std::size_t numBones = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numBones;

        //numMeshes
        std::size_t numMeshes = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numMeshes;

        //bones
        std::vector<Bone> baseSkeleton(numBones);
        FileReader::nextLine(file, buffer); //buffer = "joints {"
        for (std::size_t i = 0; i < numBones; i++) {
            FileReader::nextLine(file, buffer);
            Bone* bone = &baseSkeleton[i];
            iss.clear(); iss.str(buffer);
            iss >> bone->name >> bone->parent >> sdata >> bone->pos.X >> bone->pos.Y >> bone->pos.Z >> sdata >> bone->orient.X >> bone->orient.Y >> bone->orient.Z;
            bone->orient.computeW();
            bone->name = bone->name.substr(1, bone->name.length() - 2); //remove quote
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //mesh
        std::vector<std::unique_ptr<const ConstMesh>> constMeshes;
        for (std::size_t ii = 0; ii < numMeshes; ii++) {
            //material
            std::shared_ptr<Material> material;
            std::string materialFilename;
            FileReader::nextLine(file, buffer); //buffer= "mesh {"
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> materialFilename;
            materialFilename = materialFilename.substr(1, materialFilename.length() - 2); //remove quote
            if (materialFilename.empty() || materialFilename == "default") {
                std::vector<unsigned char> defaultAlbedoColor({177, 106, 168, 255});
                auto defaultAlbedoTexture = Texture::build(filename + " - default albedo", 1, 1, TextureFormat::RGBA_8_UINT_NORM, defaultAlbedoColor.data(), TextureDataType::INT_8);
                material = MaterialBuilder::create("defaultMaterial", defaultAlbedoTexture, false)->build();
            } else {
                material = ResourceRetriever::instance().getResource<Material>(materialFilename);
            }

            //numVertices
            std::size_t numVertices = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numVertices;

            //vertices
            std::vector<Vertex> vertices(numVertices);
            std::vector<Point2<float>> texCoords(numVertices);
            for (std::size_t i = 0; i < numVertices; i++) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                iss >> sdata >> vertices[i].linkedVerticesGroupId >> sdata >> texCoords[i].X >> texCoords[i].Y
                        >> sdata >> vertices[i].weightStart >> vertices[i].weightCount;
            }

            //numTriangles
            std::size_t numTriangles = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numTriangles;

            //triangles indices
            std::vector<std::array<uint32_t, 3>> trianglesIndices;
            trianglesIndices.reserve(numTriangles);
            for (std::size_t i = 0, triVertexIndex = 0; i < numTriangles; i++, triVertexIndex += 3) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                uint32_t index1 = 0;
                uint32_t index2 = 0;
                uint32_t index3 = 0;
                iss >> sdata >> index1 >> index2 >> index3;
                trianglesIndices.push_back({index1, index2, index3});
            }

            //numWeights
            std::size_t numWeights = 0;
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> numWeights;

            //weights
            std::vector<Weight> weights(numWeights);
            for (std::size_t i = 0; i < numWeights; i++) {
                FileReader::nextLine(file, buffer);
                iss.clear(); iss.str(buffer);
                iss >> sdata >> weights[i].boneIndex >> weights[i].bias >> sdata >> weights[i].pos.X >> weights[i].pos.Y >> weights[i].pos.Z;
            }
            FileReader::nextLine(file, buffer); //buffer= "}"

            constMeshes.push_back(std::make_unique<ConstMesh>(material, vertices, texCoords, trianglesIndices, weights, baseSkeleton));
        }

        file.close();

        return ConstMeshes::fromMeshesFile(filename, std::move(constMeshes));
    }

}
