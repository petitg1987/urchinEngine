#include <locale>
#include <stdexcept>
#include "UrchinCommon.h"

#include "loader/model/LoaderUrchinAnim.h"

namespace urchin {

    ConstAnimation* LoaderUrchinAnim::loadFromFile(const std::string& filename) {
        std::locale::global(std::locale("C")); //for float

        std::ifstream file;
        std::istringstream iss;
        std::string buffer;
        std::string sdata;

        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
        file.open(filenamePath, std::ios::in);
        if (file.fail()) {
            throw std::invalid_argument("Cannot open the file " + filenamePath + ".");
        }

        //numFrames
        unsigned int numFrames = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numFrames;

        //numBones
        unsigned int numBones = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numBones;

        //frameRates
        unsigned int frameRate = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> frameRate;

        //numAnimatedComponents
        unsigned int numAnimatedComponents = 0;
        FileReader::nextLine(file, buffer);
        iss.clear(); iss.str(buffer);
        iss >> sdata >> numAnimatedComponents;

        //hierarchy
        auto* boneInfos = new BoneInfo[numBones];
        FileReader::nextLine(file, buffer); //buffer = "hierarchy {"
        for (unsigned int i = 0; i < numBones; i++) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> boneInfos[i].name >> boneInfos[i].parent >> boneInfos[i].flags >>boneInfos[i].startIndex;
            boneInfos[i].name = boneInfos[i].name.substr(1, boneInfos[i].name.length()-2); //remove quot
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //bounds
        auto **bboxes = new AABBox<float>*[numFrames];
        FileReader::nextLine(file, buffer); //buffer = "bounds {"
        for (unsigned int i = 0; i < numFrames; i++) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);

            Point3<float> min, max;
            iss >> sdata >> min.X >> min.Y >> min.Z >> sdata >> sdata >> max.X >> max.Y >> max.Z;
            bboxes[i] = new AABBox<float>(min, max);
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //baseframe
        auto* baseFrame = new BaseFrameBone[numBones];
        FileReader::nextLine(file, buffer); //buffer = "baseframe {"
        for (unsigned int i = 0; i < numBones; i++) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> sdata >> baseFrame[i].pos.X >> baseFrame[i].pos.Y >> baseFrame[i].pos.Z >> sdata >> sdata >>baseFrame[i].orient.X >> baseFrame[i].orient.Y >> baseFrame[i].orient.Z;
            baseFrame[i].orient.computeW();
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //frames
        auto **skeletonFrames = new Bone*[numFrames];
        auto* animFrameData = new float[numAnimatedComponents];
        for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
            skeletonFrames[frameIndex] = new Bone[numBones];

            FileReader::nextLine(file, buffer); // buffer = "frame ? {"
            for (unsigned int j = 0; j < numAnimatedComponents; j++) {
                file >> animFrameData[j];
            }

            //build frame skeleton from the collected data
            for (unsigned int i = 0; i < numBones; ++i) {
                const BaseFrameBone* baseBone = &baseFrame[i];
                int j = 0;

                Point3<float> animatedPos = baseBone->pos;
                Quaternion<float> animatedOrient = baseBone->orient;

                if ((boneInfos[i].flags) & 1u) //Tx
                {
                    animatedPos.X = animFrameData[boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 2u) //Ty
                {
                    animatedPos.Y = animFrameData[boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 4u) //Tz
                {
                    animatedPos.Z = animFrameData[boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 8u) //Qx
                {
                    animatedOrient.X = animFrameData[boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 16u) //Qy
                {
                    animatedOrient.Y = animFrameData[boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 32u) //Qz
                {
                    animatedOrient.Z = animFrameData[boneInfos[i].startIndex + j];
                }

                //computes orient quaternion's w value
                animatedOrient.computeW();

                //we assume that this bone's parent has already been calculated, i.e. bone's ID should never be smaller than its parent ID.
                Bone* thisBone = &skeletonFrames[frameIndex][i];

                int parent = boneInfos[i].parent;
                thisBone->parent = parent;
                thisBone->name = boneInfos[i].name;

                //has parent ?
                if (thisBone->parent < 0) {
                    thisBone->pos = animatedPos;
                    thisBone->orient = animatedOrient;
                } else {
                    Bone* parentBone = &skeletonFrames[frameIndex][parent];

                    //adds positions
                    Point3<float> rpos = parentBone->orient.rotatePoint(animatedPos); //rotated position
                    thisBone->pos.X = rpos.X + parentBone->pos.X;
                    thisBone->pos.Y = rpos.Y + parentBone->pos.Y;
                    thisBone->pos.Z = rpos.Z + parentBone->pos.Z;

                    //concatenates rotations
                    thisBone->orient = (parentBone->orient * animatedOrient).normalize();
                }
            }

            FileReader::nextLine(file, buffer); //buferf = "}"
        }

        delete[] boneInfos;
        delete[] baseFrame;
        delete[] animFrameData;

        file.close();
        return (new ConstAnimation(filename, numFrames, numBones, frameRate, skeletonFrames, bboxes));
    }
}
