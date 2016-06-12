#include <map>
#include <sstream>
#include <limits>

#include "tools/debug/DebugRecorder.h"

namespace urchin
{

	DebugRecorder::DebugRecorder(const std::string &filename)
	{
		file.open(filename, std::ofstream::out | std::ofstream::trunc);
	}

	DebugRecorder::~DebugRecorder()
	{
		if(file.is_open())
		{
			file.close();
		}
	}


	void DebugRecorder::recordPoint(const std::string &entityId, const Point3<float>& point)
	{
		std::ostringstream oss;
		oss.precision(std::numeric_limits<float>::max_digits10);

		oss<<point.X<<" "<<point.Y<<" "<<point.Z<<std::endl;

		writeAction(std::string(POINT3_ACTION) + ": " + entityId + "\n" + oss.str());
	}

	void DebugRecorder::recordConvexHull(const std::string &entityId, const ConvexHull3D<float> &convexHull)
	{
		std::ostringstream oss;
		oss.precision(std::numeric_limits<float>::max_digits10);

		oss<<NUM_INDEXED_TRIANGLES<<": "<<convexHull.getIndexedPoints().size()<<std::endl;
		for(auto  &it : convexHull.getIndexedPoints())
		{
			oss<<it.first<<": "<<it.second.X<<" "<<it.second.Y<<" "<<it.second.Z<<std::endl;
		}

		oss<<NUM_INDEXED_POINTS<<": "<<convexHull.getIndexedTriangles().size()<<std::endl;
		for(auto  &it : convexHull.getIndexedTriangles())
		{
			oss<<it.first<<": "<<it.second.getIndexes()[0]<<" "<<it.second.getIndexes()[1]<<" "<<it.second.getIndexes()[2]<<std::endl;
		}

		writeAction(std::string(CONVEX_HULL_3D_ACTION) + ": " + entityId + "\n" + oss.str());
	}

	void DebugRecorder::clearEntity(const std::string &entityId)
	{
		std::string content = std::string(CLEAR_ENTITY_ACTION) + ": " + entityId + "\n";
		writeAction(content);
	}

	void DebugRecorder::writeAction(const std::string &content)
	{
		std::string actionContent = ">" + content + "\n";
		file.write(actionContent.c_str(), actionContent.size());
	}

}
