#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class StringUtilTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void splitString();
        void splitEmptyString();
        void mergeString();

        void camelToKebabCase();
        void kebabToCamelCase();
};
