#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <Urchin3dEngine.h>

class Renderer3dTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void modelPreWarm();

    private:
        std::unique_ptr<urchin::Model> buildModel(std::string, const std::string&, const urchin::Point3<float>&) const;
};
