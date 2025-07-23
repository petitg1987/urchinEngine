#include <locale>
#include <stdexcept>
#include <UrchinCommon.h>

#include "loader/model/LoaderUrchinAnim.h"

namespace urchin {

    std::shared_ptr<ConstAnimation> LoaderUrchinAnim::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        std::istringstream iss;
        iss.imbue(std::locale::classic());
        std::string buffer;
        std::string sdata;

        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filename);
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
        std::vector<BoneInfo> boneInfos(numBones);
        FileReader::nextLine(file, buffer); //buffer = "hierarchy {"
        for (auto& boneInfo : boneInfos) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> boneInfo.name >> boneInfo.parent >> boneInfo.flags >>boneInfo.startIndex;
            boneInfo.name = boneInfo.name.substr(1, boneInfo.name.length() - 2); //remove quote
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //bounds
        std::vector<AABBox<float>> bboxes;
        bboxes.reserve(numFrames);
        FileReader::nextLine(file, buffer); //buffer = "bounds {"
        for (unsigned int i = 0; i < numFrames; i++) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);

            Point3<float> min;
            Point3<float> max;
            iss >> min.X >> min.Y >> min.Z >> sdata >> max.X >> max.Y >> max.Z;
            bboxes.emplace_back(min, max);
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //baseframe
        std::vector<BaseFrameBone> baseFrame(numBones);
        FileReader::nextLine(file, buffer); //buffer = "baseframe {"
        for (auto& bf : baseFrame) {
            FileReader::nextLine(file, buffer);
            iss.clear(); iss.str(buffer);
            iss >> bf.pos.X >> bf.pos.Y >> bf.pos.Z >> sdata >> bf.orient.X >> bf.orient.Y >> bf.orient.Z;
            bf.orient.computeW();
        }
        FileReader::nextLine(file, buffer); //buffer = "}"

        //frames
        std::vector<std::vector<Bone>> skeletonFrames(numFrames);
        std::vector<float> animFrameData(numAnimatedComponents);
        for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
            skeletonFrames[frameIndex].resize(numBones);

            FileReader::nextLine(file, buffer); // buffer = "frame ? {"
            for (float& afd : animFrameData) {
                file >> afd;
            }

            //build frame skeleton from the collected data
            for (unsigned int i = 0; i < numBones; ++i) {
                const BaseFrameBone& baseBone = baseFrame[i];
                std::size_t j = 0;

                Point3<float> animatedPos = baseBone.pos;
                Quaternion<float> animatedOrient = baseBone.orient;

                if ((boneInfos[i].flags) & 1u) { //Tx
                    animatedPos.X = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 2u) { //Ty
                    animatedPos.Y = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 4u) { //Tz
                    animatedPos.Z = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 8u) { //Qx
                    animatedOrient.X = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 16u) { //Qy
                    animatedOrient.Y = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
                    ++j;
                }

                if ((boneInfos[i].flags) & 32u) { //Qz
                    animatedOrient.Z = animFrameData[(std::size_t)boneInfos[i].startIndex + j];
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
                    const Bone* parentBone = &skeletonFrames[frameIndex][(std::size_t)parent];

                    //adds positions
                    Point3<float> rotatedPosition = parentBone->orient.rotatePoint(animatedPos);
                    thisBone->pos.X = rotatedPosition.X + parentBone->pos.X;
                    thisBone->pos.Y = rotatedPosition.Y + parentBone->pos.Y;
                    thisBone->pos.Z = rotatedPosition.Z + parentBone->pos.Z;

                    //concatenates rotations
                    thisBone->orient = (parentBone->orient * animatedOrient).normalize();
                }

                if (thisBone->pos.isEqual(baseBone.pos, 0.0001f) && thisBone->orient.isEqualOrientation(baseBone.orient, 0.0001f)) {
                    thisBone->sameAsBasePose = true;
                } else {
                    thisBone->sameAsBasePose = false;
                }
            }

            FileReader::nextLine(file, buffer); //buferf = "}"
        }

        file.close();
        return std::make_shared<ConstAnimation>(filename, numFrames, numBones, frameRate, skeletonFrames, std::move(bboxes));
    }
}
