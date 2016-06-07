#include <map>
#include <sstream>

#include "visualizer/DebugRecorder.h"

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

		writeAction("POINT3: " + entityId + "\n" + oss.str());
	}

	void DebugRecorder::recordConvexHull(const std::string &entityId, const ConvexHull3D<float> &convexHull)
	{
		std::ostringstream oss;
		oss.precision(std::numeric_limits<float>::max_digits10);

		const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull.getIndexedTriangles();
		const std::map<unsigned int, Point3<float>> &indexedPoints = convexHull.getIndexedPoints();


		for(std::map<unsigned int, IndexedTriangle3D<float>>::const_iterator it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it)
		{
			IndexedTriangle3D<float> triangle = it->second;

			Point3<float> point0 = indexedPoints.find(triangle.getIndexes()[0])->second;
			Point3<float> point1 = indexedPoints.find(triangle.getIndexes()[1])->second;
			Point3<float> point2 = indexedPoints.find(triangle.getIndexes()[2])->second;
			oss<<"Triangle"<<it->first<<": "<<point0.X<<" "<<point0.Y<<" "<<point1.Z<<"; "
											<<point1.X<<" "<<point1.Y<<" "<<point1.Z<<"; "
											<<point2.X<<" "<<point2.Y<<" "<<point2.Z<<std::endl;
		}

		writeAction("CONVEXHULL3D: " + entityId + "\n" + oss.str());
	}

	void DebugRecorder::clearEntity(const std::string &entityId)
	{
		std::string content = "CLEAR_ENTITY: " + entityId + "\n";
		writeAction(content);
	}

	void DebugRecorder::clearAllEntities()
	{
		std::string content = "CLEAR_ALL_ENTITIES\n";
		writeAction(content);
	}

	void DebugRecorder::writeAction(const std::string &content)
	{
		std::string actionContent = ">" + content + "\n";
		file.write(actionContent.c_str(), actionContent.size());
	}

}
