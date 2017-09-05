#ifndef URCHINENGINE_URCHINAIENGINE_H
#define URCHINENGINE_URCHINAIENGINE_H

#include "AIManager.h"

#include "input/AIWorld.h"
#include "input/AIObject.h"

#include "path/navmesh/NavMeshGenerator.h"
#include "path/navmesh/model/NavMeshConfig.h"
#include "path/navmesh/model/NavMeshAgent.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/model/NavTriangle.h"
#include "path/navmesh/triangulation/MonotonePolygonAlgorithm.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"
#include "path/navmesh/triangulation/TriangulationAlgorithm.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsIntersection.h"
#include "path/navmesh/csg/PolygonsSubtraction.h"
#include "path/navmesh/csg/CSGPolygon.h"

#endif
