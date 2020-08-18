#ifndef URCHINENGINE_ORTHOGONALPROJECTIONTEST_H
#define URCHINENGINE_ORTHOGONALPROJECTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include "UrchinCommon.h"

class OrthogonalProjectionTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void orthogonalProjectionLine2D();
        void orthogonalProjectionLine3D();

        void orthogonalProjectionPlane();
};

#endif
