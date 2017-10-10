#include <locale>
#include <stdexcept>
#include "UrchinCommon.h"

#include "loader/model/LoaderUrchinAnim.h"

namespace urchin
{

	void LoaderUrchinAnim::nextLine(std::ifstream &file, std::string &buffer)
	{
		do
		{
			std::getline(file, buffer);

			//delete '\r'
			unsigned long length = buffer.length()-1;
			if(length >=0 && buffer[length]=='\r')
			{
				buffer.resize(length);
			}

		}while(buffer.length()==0 && !file.eof());
	}

	ConstAnimation *LoaderUrchinAnim::loadFromFile(const std::string &filename, void *params)
	{
		std::locale::global(std::locale("C")); //for float
		
		std::ifstream file;
		std::istringstream iss;
		std::string buffer;
		std::string sdata;
		
		std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
		file.open(filenamePath, std::ios::in);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + filenamePath + ".");
		}
		
		//numFrames
		unsigned int numFrames;
		nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> numFrames;
		
		//numBones
		unsigned int numBones;
		nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> numBones;
		
		//frameRates
		unsigned int frameRate;
		nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> frameRate;
		
		//numAnimatedComponents
		unsigned int numAnimatedComponents;
		nextLine(file, buffer);
		iss.clear(); iss.str(buffer);
		iss >> sdata >> numAnimatedComponents;
		
		//hierarchy
		auto *boneInfos = new BoneInfo[numBones];
		nextLine(file, buffer); //buffer = "hierarchy {"
		for(unsigned int i=0;i<numBones;i++)
		{
			nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> boneInfos[i].name >> boneInfos[i].parent >> boneInfos[i].flags >>boneInfos[i].startIndex;
			boneInfos[i].name =boneInfos[i].name.substr(1, boneInfos[i].name.length()-2); //remove quot
		}
		nextLine(file, buffer); //buffer = "}"
		
		//bounds
		auto **bboxes = new AABBox<float>*[numFrames];
		nextLine(file, buffer); //buffer = "bounds {"
		for(unsigned int i=0;i<numFrames;i++)
		{
			nextLine(file, buffer);
			iss.clear(); iss.str(buffer);

			Point3<float> min, max;
			iss >> sdata >> min.X >> min.Y >> min.Z >> sdata >> sdata >> max.X >> max.Y >> max.Z;
			bboxes[i] = new AABBox<float>(min, max);
		}	
		nextLine(file, buffer); //buffer = "}"
		
		//baseframe
		auto *baseFrame = new BaseFrameBone[numBones];
		nextLine(file, buffer); //buffer = "baseframe {"
		for(unsigned int i=0;i<numBones;i++)
		{
			nextLine(file, buffer);
			iss.clear(); iss.str(buffer);
			iss >> sdata >> baseFrame[i].pos.X >> baseFrame[i].pos.Y >> baseFrame[i].pos.Z >> sdata >> sdata >>baseFrame[i].orient.X >> baseFrame[i].orient.Y >> baseFrame[i].orient.Z;
			baseFrame[i].orient.computeW();
		}
		nextLine(file, buffer); //buffer = "}"
		
		//frames
		auto **skeletonFrames = new Bone*[numFrames];
		auto *animFrameData = new float[numAnimatedComponents];
		for(unsigned int frameIndex=0;frameIndex<numFrames;++frameIndex)
		{
			skeletonFrames[frameIndex] = new Bone[numBones];
		
			nextLine(file, buffer); // buffer = "frame ? {"
			for(unsigned int j=0;j<numAnimatedComponents;j++)
			{
				file >> animFrameData[j];
			}
			
			//build frame skeleton from the collected data
			for(unsigned int i=0; i<numBones; ++i)
			{
				const BaseFrameBone *baseBone = &baseFrame[i];
				int j = 0;

				Point3<float> animatedPos = baseBone->pos;
				Quaternion<float> animatedOrient = baseBone->orient;

				if(boneInfos[i].flags & 1) //Tx
				{
					animatedPos.X = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				if(boneInfos[i].flags & 2) //Ty
				{
					animatedPos.Y = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				if(boneInfos[i].flags & 4) //Tz
				{
					animatedPos.Z = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				if(boneInfos[i].flags & 8) //Qx
				{
					animatedOrient.X = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				if(boneInfos[i].flags & 16) //Qy
				{
					animatedOrient.Y = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				if(boneInfos[i].flags & 32) //Qz
				{
					animatedOrient.Z = animFrameData[boneInfos[i].startIndex + j];
					++j;
				}

				//computes orient quaternion's w value
				animatedOrient.computeW();

				//we assume that this bone's parent has already been calculated, i.e. bone's ID should never be smaller than its parent ID.
				Bone *thisBone = &skeletonFrames[frameIndex][i];

				int parent = boneInfos[i].parent;
				thisBone->parent = parent;
				thisBone->name = boneInfos[i].name;

				//has parent ?
				if(thisBone->parent < 0)
				{
					thisBone->pos = animatedPos;
					thisBone->orient = animatedOrient;
				}else
				{
					Bone *parentBone = &skeletonFrames[frameIndex][parent];

					//adds positions
					Point3<float> rpos = parentBone->orient.rotatePoint(animatedPos); //rotated position
					thisBone->pos.X = rpos.X + parentBone->pos.X;
					thisBone->pos.Y = rpos.Y + parentBone->pos.Y;
					thisBone->pos.Z = rpos.Z + parentBone->pos.Z;

					//concatenates rotations
					thisBone->orient = (parentBone->orient * animatedOrient).normalize();
				}
			}

			nextLine(file, buffer); //buferf = "}"
		}

		delete [] boneInfos;
		delete [] baseFrame;
		delete [] animFrameData;

		file.close();
		return (new ConstAnimation(filename, numFrames, numBones, frameRate, skeletonFrames, bboxes));
	}
}
