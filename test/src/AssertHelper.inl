inline void AssertHelper::assertTrue(bool b, const std::string& msg) {
    if (msg.empty()) {
        CPPUNIT_ASSERT(b);
    } else {
        CPPUNIT_ASSERT_MESSAGE(msg, b);
    }
}

inline void AssertHelper::assertFalse(bool b, const std::string& msg) {
    assertTrue(!b, msg);
}

inline void AssertHelper::assertNull(const void* object) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Object is not null", object == nullptr);
}

inline void AssertHelper::assertNotNull(const void* object) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Object is null", object != nullptr);
}

inline void AssertHelper::assertObjectEquals(const void* object, const void* expectedObject) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Objects are different", object == expectedObject);
}

inline void AssertHelper::assertStringEquals(const std::string& value, const std::string& expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + value + ", expected: " + expected, value == expected);
}

inline void AssertHelper::assertIntEquals(int value, int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

inline void AssertHelper::assertUnsignedIntEquals(unsigned int value, unsigned int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

inline void AssertHelper::assertUnsignedIntEquals(std::size_t value, std::size_t expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

inline void AssertHelper::assert3SizesEquals(const std::array<std::size_t, 3>& value, const std::array<std::size_t, 3>& expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

inline void AssertHelper::assertFloatEquals(float value, float expected, float epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + floatToString(value) + ", expected: " + floatToString(expected), urchin::MathFunction::isEqual(value, expected, epsilon));
}

inline void AssertHelper::assertPoint2FloatEquals(const urchin::Point2<float>& value, const urchin::Point2<float>& expected, float epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + urchin::StringUtil::toString(value) + ", expected: " + urchin::StringUtil::toString(expected), value.isEqual(expected, epsilon));
}

inline void AssertHelper::assertPoint2IntEquals(const urchin::Point2<int>& value, const urchin::Point2<int>& expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + urchin::StringUtil::toString(value) + ", expected: " + urchin::StringUtil::toString(expected), value.isEqual(expected, 0));
}

inline void AssertHelper::assertPoints2FloatEquals(std::span<const urchin::Point2<float>> points, std::span<const urchin::Point2<float>> expectedPoints, float epsilon) {
    assertUnsignedIntEquals(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint2FloatEquals(points[i], expectedPoints[i], epsilon);
    }
}

inline void AssertHelper::assertPoint3FloatEquals(const urchin::Point3<float>& value, const urchin::Point3<float>& expected, float epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + urchin::StringUtil::toString(value) + ", expected: " + urchin::StringUtil::toString(expected), value.isEqual(expected, epsilon));
}

inline void AssertHelper::assertPoints3FloatEquals(std::span<const urchin::Point3<float>> points, std::span<const urchin::Point3<float>> expectedPoints, float epsilon) {
    assertUnsignedIntEquals(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint3FloatEquals(points[i], expectedPoints[i], epsilon);
    }
}

inline void AssertHelper::assertPoint3IntEquals(const urchin::Point3<int>& value, const urchin::Point3<int>& expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + urchin::StringUtil::toString(value) + ", expected: " + urchin::StringUtil::toString(expected), value.isEqual(expected, 0));
}

inline void AssertHelper::assertVector3FloatEquals(const urchin::Vector3<float>& value, const urchin::Vector3<float>& expected, float epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + urchin::StringUtil::toString(value) + ", expected: " + urchin::StringUtil::toString(expected), value.isEqual(expected, epsilon));
}

inline void AssertHelper::assertQuaternionFloatEquals(const urchin::Quaternion<float>& value, const urchin::Quaternion<float>& expected, float epsilon) {
    urchin::Quaternion qTest1(value);
    if ((value.W < 0.0f && expected.W > 0.0f) || (value.W > 0.0f && expected.W < 0.0f)) {
        qTest1.X = -qTest1.X;
        qTest1.Y = -qTest1.Y;
        qTest1.Z = -qTest1.Z;
        qTest1.W = -qTest1.W;
    }

    for (unsigned int i = 0; i < 4; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(qTest1[i]) + ", expected: " + floatToString(expected[i]),
                               urchin::MathFunction::isEqual(qTest1[i], expected[i], epsilon));
    }
}

inline std::string AssertHelper::floatToString(float f) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(10) << f;
    return stream.str();
}

