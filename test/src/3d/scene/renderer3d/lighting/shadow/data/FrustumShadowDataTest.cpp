#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

#include "FrustumShadowDataTest.h"
#include "AssertHelper.h"
using namespace urchin;

void FrustumShadowDataTest::test() { //TODO impl
    auto modelInLightView = std::make_unique<Model>("");
    modelInLightView->setPosition(Point3<float>(5.0, 2.0, 3.0));
    auto modelOctreeManager = std::make_unique<OctreeManager<Model>>(9999.0);
    modelOctreeManager->addOctreeable(modelInLightView.get());
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto shadowData = std::make_unique<ShadowData>(light.get(), modelOctreeManager.get(), 1);
    auto frustumShadowData = std::make_unique<FrustumShadowData>(shadowData.get());

    frustumShadowData->update(Frustum<float>(90.0f, 1.0f, 0.01f, 1000.0f), false);

    AABBox<float> result = frustumShadowData->getShadowCasterReceiverBox();
    AssertHelper::assertPoint3FloatEquals(result.getMin(), Point3<float>(2.5f, 1.5f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(result.getMax(), Point3<float>(3.5f, 2.5f, 0.0f));
}

CppUnit::Test* FrustumShadowDataTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller<FrustumShadowDataTest>("test", &FrustumShadowDataTest::test));

    return suite;
}
