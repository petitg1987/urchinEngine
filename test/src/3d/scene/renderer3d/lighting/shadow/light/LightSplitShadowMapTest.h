#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include <Urchin3dEngine.h>

class LightSplitShadowMapTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void modelsInFrustumSplit();
        void modelsOutsideFrustumSplit();
        void modelOutsideFrustumProducingShadow();

    private:
        std::unique_ptr<urchin::ModelOcclusionCuller> buildModelOcclusionCuller(const std::vector<urchin::Point3<float>> &);
};
