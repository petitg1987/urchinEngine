void AssertHelper::assertTrue(bool b, const std::string& msg) {
    if (msg.empty()) {
        CPPUNIT_ASSERT(b);
    } else {
        CPPUNIT_ASSERT_MESSAGE(msg, b);
    }
}

void AssertHelper::assertObjectEquals(void* object, void* expectedObject) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Objects are different", object == expectedObject);
}

void AssertHelper::assertStringEquals(const std::string& value, const std::string& expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + value + ", expected: " + expected, value == expected);
}

void AssertHelper::assertIntEquals(int value, int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assert3IntsEquals(const int* value, std::array<int, 3> expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertUnsignedIntEquals(unsigned int value, unsigned int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assertUnsignedIntEquals(std::size_t value, std::size_t expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assert3UnsignedIntsEquals(const unsigned int* value, std::array<unsigned int, 3> expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assert3SizesEquals(const std::size_t* value, std::array<std::size_t, 3> expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertFloatEquals(float value, float expected, float epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + floatToString(value) + ", expected: " + floatToString(expected),
                           urchin::MathFunction::isEqual(value, expected, epsilon));
}

void AssertHelper::assertPoint2FloatEquals(const urchin::Point2<float>& value, const urchin::Point2<float>& expected, float epsilon) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                               urchin::MathFunction::isEqual(value[i], expected[i], epsilon));
    }
}

void AssertHelper::assertPoint2IntEquals(const urchin::Point2<int>& value, const urchin::Point2<int>& expected) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertPoint2LongLongEquals(const urchin::Point2<long long>& value, const urchin::Point2<long long>& expected) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertPoints2FloatEquals(const std::vector<urchin::Point2<float>>& points, const std::vector<urchin::Point2<float>>& expectedPoints, float epsilon) {
    AssertHelper::assertUnsignedIntEquals(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint2FloatEquals(points[i], expectedPoints[i], epsilon);
    }
}

void AssertHelper::assertPoints2LongLongEquals(const std::vector<urchin::Point2<long long>>& points, const std::vector<urchin::Point2<long long>>& expectedPoints) {
    AssertHelper::assertUnsignedIntEquals(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint2LongLongEquals(points[i], expectedPoints[i]);
    }
}

void AssertHelper::assertPoint3FloatEquals(const urchin::Point3<float>& value, const urchin::Point3<float>& expected, float epsilon) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                               urchin::MathFunction::isEqual(value[i], expected[i], epsilon));
    }
}

void AssertHelper::assertVector3FloatEquals(const urchin::Vector3<float>& value, const urchin::Vector3<float>& expected, float epsilon) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                               urchin::MathFunction::isEqual(value[i], expected[i], epsilon));
    }
}

void AssertHelper::assertQuaternionFloatEquals(const urchin::Quaternion<float>& value, const urchin::Quaternion<float>& expected, float epsilon) {
    urchin::Quaternion<float> qTest1(value);
    if ((value.W < 0.0 && expected.W > 0.0) || (value.W > 0.0 && expected.W < 0.0)) {
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

void AssertHelper::assertPolygonFloatEquals(std::vector<urchin::Point2<float>> orientedPoints, const std::vector<urchin::Point2<float>>& orientedExpectedPoints, float epsilon) {
    AssertHelper::assertUnsignedIntEquals(orientedPoints.size(), orientedExpectedPoints.size());

    for (std::size_t i = 0; i < orientedPoints.size(); ++i) {
        try {
            assertPoints2FloatEquals(orientedPoints, orientedExpectedPoints, epsilon);
        } catch (std::exception& e) {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

void AssertHelper::assertPolygonLongLongEquals(std::vector<urchin::Point2<long long>> orientedPoints, const std::vector<urchin::Point2<long long>>& orientedExpectedPoints) {
    AssertHelper::assertUnsignedIntEquals(orientedPoints.size(), orientedExpectedPoints.size());

    for (std::size_t i = 0; i < orientedPoints.size(); ++i) {
        try {
            assertPoints2LongLongEquals(orientedPoints, orientedExpectedPoints);
        } catch (std::exception& e) {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

std::string AssertHelper::floatToString(float f) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(10) << f;
    return stream.str();
}

