#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <Urchin3dEngine.h>

class ModelOcclusionCullerTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void movingModel();
        void updateCullBehavior();

    private:
        std::unique_ptr<urchin::Model> buildModel(const urchin::Point3<float>&) const;
};
