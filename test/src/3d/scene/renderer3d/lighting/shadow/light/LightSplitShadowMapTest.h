#ifndef URCHINENGINE_LIGHTSPLITSHADOWMAPTEST_H
#define URCHINENGINE_LIGHTSPLITSHADOWMAPTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

class LightSplitShadowMapTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void modelsInFrustumSplit();
        void modelsOutsideFrustumSplit();

    private:
        std::unique_ptr<urchin::OctreeManager<urchin::Model>> buildModelOctreeManager(const std::vector<urchin::Point3<float>> &);
        void cleanOctreeManager(const std::unique_ptr<urchin::OctreeManager<urchin::Model>>&);
};

#endif
