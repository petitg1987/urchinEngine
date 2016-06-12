#include <sstream>
#include <iostream>
#include <stdexcept>
#include "UrchinCommon.h"

#include "ActionReader.h"
#include "scene/controller/action/Point3Action.h"
#include "scene/controller/action/ConvexHull3DAction.h"
#include "scene/controller/action/ClearEntityAction.h"

namespace urchin
{

	ActionReader::ActionReader(const std::string &filename) :
			filename(filename)
	{

	}

	ActionReader::~ActionReader()
	{

	}

	std::vector<std::shared_ptr<Action>> ActionReader::readDebugFile() const
	{
		std::vector<std::shared_ptr<Action>> actions;

		std::ifstream file;
		file.open(filename, std::ios::in);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + filename + ".");
		}

		do
		{
			std::string buffer;
			nextLine(file, buffer);

			std::istringstream iss(buffer);
			std::string rawActionType, entityId;
			iss >> rawActionType >> entityId;

			if(rawActionType.length()<=2)
			{
				continue;
			}else if(rawActionType.substr(0, 1).compare(">")!=0 || rawActionType.substr(rawActionType.length()-1, 1).compare(":")!=0)
			{
				throw std::runtime_error("Unknown action type: " + rawActionType);
			}
			std::string actionType = rawActionType.substr(1, rawActionType.length()-2);

			if(entityId.length()==0)
			{
				throw std::runtime_error("Empty entity ID found for action type: " + actionType);
			}

			std::shared_ptr<Action> action = readAction(file, actionType, entityId);
			actions.push_back(action);

		}while(!file.eof());

		return actions;
	}

	void ActionReader::nextLine(std::ifstream &file, std::string &buffer) const
	{
		do
		{
			std::getline(file, buffer);

			//delete '\r'
			int length = buffer.length()-1;
			if(length >=0 && buffer[length]=='\r')
			{
				buffer.resize(length);
			}

		}while(buffer.length()==0 && !file.eof());
	}

	std::shared_ptr<Action> ActionReader::readAction(std::ifstream &file, const std::string &actionType, const std::string &entityId) const
	{
		std::locale::global(std::locale("C")); //for float

		if(actionType.compare(POINT3_ACTION)==0)
		{
			return readPoint3Action(file, entityId);
		}else if(actionType.compare(CONVEX_HULL_3D_ACTION)==0)
		{
			return readConvexHull3DAction(file, entityId);
		}else if(actionType.compare(CLEAR_ENTITY_ACTION)==0)
		{
			return readClearEntityAction(entityId);
		}

		throw std::runtime_error("Impossible to handle action type: " + actionType);
	}

	std::shared_ptr<Action> ActionReader::readPoint3Action(std::ifstream &file, const std::string &entityId) const
	{
		std::string buffer;
		nextLine(file, buffer);

		Point3<float> point = Converter::toPoint3(buffer);

		return std::make_shared<Point3Action>(entityId, point);
	}

	std::shared_ptr<Action> ActionReader::readConvexHull3DAction(std::ifstream &file, const std::string &entityId) const
	{
		std::string buffer;
		std::istringstream iss;

		//indexed points
		std::map<unsigned int, Point3<float>> indexedPoints;

		nextLine(file, buffer);
		iss.clear();
		iss.str(buffer);
		std::string labelIndexedPoints, numIndexedPointsStr;
		iss >> labelIndexedPoints >> numIndexedPointsStr;

		unsigned int numIndexedPoints = Converter::toUnsignedInt(numIndexedPointsStr);
		for(unsigned int i=0; i<numIndexedPoints; ++i)
		{
			nextLine(file, buffer);
			iss.clear();
			iss.str(buffer);
			std::string indexStr;
			float pointX, pointY, pointZ;
			iss >> indexStr >> pointX >> pointY >> pointZ;

			unsigned int index = Converter::toUnsignedInt(indexStr.substr(0, indexStr.length()-1));
			Point3<float> point(pointX, pointY, pointZ);
			indexedPoints.insert(std::pair<unsigned int,Point3<float>>(index, point));
		}

		//indexed triangles
		std::map<unsigned int, IndexedTriangle3D<float>> indexedTriangles;

		nextLine(file, buffer);
		iss.clear();
		iss.str(buffer);
		std::string labelIndexedTriangles, numIndexedTrianglesStr;
		iss >> labelIndexedTriangles >> numIndexedTrianglesStr;

		unsigned int numIndexedTriangles = Converter::toUnsignedInt(numIndexedTrianglesStr);
		for(unsigned int i=0; i<numIndexedTriangles; ++i)
		{
			nextLine(file, buffer);
			iss.clear();
			iss.str(buffer);
			std::string indexStr;
			unsigned int index0, index1, index2;
			iss >> indexStr >> index0 >> index1 >> index2;

			unsigned int index = Converter::toUnsignedInt(indexStr.substr(0, indexStr.length()-1));
			IndexedTriangle3D<float> indexedTriangle(index0, index1, index2);
			indexedTriangles.insert(std::pair<unsigned int,IndexedTriangle3D<float>>(index, indexedTriangle));
		}

		ConvexHull3D<float> convexHull(indexedPoints, indexedTriangles);
		return std::make_shared<ConvexHull3DAction>(entityId, convexHull);
	}

	std::shared_ptr<Action> ActionReader::readClearEntityAction(const std::string &entityId) const
	{
		return std::make_shared<ClearEntityAction>(entityId);
	}

}
