#ifndef ENGINE_DEBUGRECORDER_H
#define ENGINE_DEBUGRECORDER_H

#include <string>
#include <fstream>
#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Debug recorder can be setup on some physics process in order to record debug data. The record produced can be then used in a
	* debug visualizer to replay the process step by step.
	*/
	class DebugRecorder
	{
		public:
			DebugRecorder(const std::string &);
			~DebugRecorder();

			void recordPoint(const std::string &, const Point3<float> &);
			void recordConvexHull(const std::string &, const ConvexHull3D<float> &);

			void clearEntity(const std::string &);
			void clearAllEntities();

		private:
			std::ofstream file;

			void writeAction(const std::string &);
	};

}

#endif
