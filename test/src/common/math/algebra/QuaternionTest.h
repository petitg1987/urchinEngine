#ifndef URCHINENGINE_QUATERNIONTEST_H
#define URCHINENGINE_QUATERNIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class QuaternionTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void multiplyAxisAngleQuaternions9And45();
        void multiplyLookAtQuaternions9And45();
        void multiplyLookAtQuaternions180And45();
        void multiplyLookAtQuaternions180And45UpDown();

        void eulerXYZ();
        void eulerXZY();
        void eulerYXZ();
        void eulerYZX();
        void eulerZXY();
        void eulerZYX();

        void eulerXYX();
        void eulerXZX();
        void eulerYXY();
        void eulerYZY();
        void eulerZXZ();
        void eulerZYZ();

        void slerp50Rotation();
        void slerp25Rotation();
        void slerpShortestPath();
        void lerp50Rotation();
        void lerp25Rotation();
        void lerpShortestPath();

        void toAxisAngle90();
};

#endif
