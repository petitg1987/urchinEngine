#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <vector>

#include <UrchinPhysicsEngine.h>

class IslandContainerTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void cascadeMergeIslands();
        void mergeAllIslands();
        void createTwoSeparateIslands();
};

class TestBody : public urchin::IslandElement {
    public:
        bool isActive() const override {
            return true;
        }
};
