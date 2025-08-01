#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class StringUtilTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void splitStringByChar();
        void splitEmptyStringByChar();
        void splitStringByString();
        void splitUniqueStringByString();
        void splitEmptyStringByString();
        void joinString();

        void camelToKebabCase();
        void kebabToCamelCase();
};
