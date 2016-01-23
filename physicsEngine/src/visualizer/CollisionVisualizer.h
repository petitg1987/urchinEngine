#ifndef ENGINE_COLLISIONVISUALIZER_H
#define ENGINE_COLLISIONVISUALIZER_H

#include <vector>
#include <mutex>
#include "UrchinCommon.h"

#include "collision/CollisionWorld.h"
#include "collision/ManifoldResult.h"

namespace urchin
{

	#ifdef _DEBUG
		/**
		* Collision visualizer: can be used to debug problems in collision process. This class stock all data coming from collision process in
		* order to be displayed in a graphical engine
		*/
		class CollisionVisualizer : public Observer
		{
			public:
				CollisionVisualizer(CollisionWorld *);
				~CollisionVisualizer();

				void notify(Observable *, int);

				const std::vector<ManifoldResult> &getManifoldResult() const;

			private:
				CollisionWorld *collisionWorld;
				mutable std::mutex visualizerDataMutex;

				std::vector<ManifoldResult> manifoldResults;
		};
	#endif

}

#endif
