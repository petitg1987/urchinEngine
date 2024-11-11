#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include <UrchinPhysicsEngine.h>

class IslandContainerTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void cascadeMergeIslands();
        void mergeAllIslands();
        void createTwoSeparateIslands();
};

class TestBody final : public urchin::IslandElement {
    public:
        bool isActive() const override {
            return true;
        }
};
