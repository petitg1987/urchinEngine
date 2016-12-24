#ifndef ENGINE_MAPEDITOR_AICONTROLLER_H
#define ENGINE_MAPEDITOR_AICONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class AIController
	{
		public:
			AIController(MapHandler *);
			~AIController();

			bool isModified() const;
			void resetModified();

			void generateNavMesh(float, float);

		private:
			void markModified();

			bool bIsModified;
			MapHandler *mapHandler;
	};

}

#endif
