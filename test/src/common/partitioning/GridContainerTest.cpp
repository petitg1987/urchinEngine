#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include "common/partitioning/GridContainerTest.h"
#include "AssertHelper.h"
using namespace urchin;

MyGridItem::MyGridItem(const Point3<int>& position) :
        position(position) {
}

const Point3<int>& MyGridItem::getGridPosition() const {
    return position;
}

void GridContainerTest::findXPositiveNeighbor() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3(4, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3(5, 1, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighbor117 = gridContainer.findNeighbor(Point3(1, 1, 7), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);

    AssertHelper::assertTrue(neighbor117 == item1);
    AssertHelper::assertTrue(neighborItem1 == item3);
    AssertHelper::assertNull(neighborItem2.get());
    AssertHelper::assertTrue(neighborItem3 == item4);
    AssertHelper::assertNull(neighborItem4.get());
}

void GridContainerTest::findXNegativeNeighbor() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3(4, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3(5, 1, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighbor317 = gridContainer.findNeighbor(Point3(3, 1, 7), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);

    AssertHelper::assertTrue(neighbor317 == item1);
    AssertHelper::assertNull(neighborItem1.get());
    AssertHelper::assertNull(neighborItem2.get());
    AssertHelper::assertTrue(neighborItem3 == item1);
    AssertHelper::assertTrue(neighborItem4 == item3);
}

void GridContainerTest::findYPositiveAllDirectNeighbors() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3(2, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3(2, 3, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3(2, 5, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighborsItem1 = gridContainer.findAllDirectNeighbors(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);

    AssertHelper::assertUnsignedIntEquals(neighborsItem1.size(), 2);
    AssertHelper::assertTrue(neighborsItem1[0] == item2);
    AssertHelper::assertTrue(neighborsItem1[1] == item3);
}

void GridContainerTest::findYNegativeAllDirectNeighbors() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3(2, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3(2, 3, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3(2, 5, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighborsItem2 = gridContainer.findAllDirectNeighbors(item2->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::NEGATIVE);

    AssertHelper::assertUnsignedIntEquals(neighborsItem2.size(), 1);
    AssertHelper::assertTrue(neighborsItem2[0] == item1);
}

void GridContainerTest::removeNeighbor() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3(2, 2, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);

    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertTrue(neighborItem1 == item2);
    gridContainer.removeItem(item2->getGridPosition());

    neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertNull(neighborItem1.get());
}

void GridContainerTest::checkItemExist() {
    auto item1 = std::make_shared<MyGridItem>(Point3(2, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);

    bool is217Exist = gridContainer.isItemExist(Point3(2, 1, 7));
    bool is317Exist = gridContainer.isItemExist(Point3(3, 1, 7));

    AssertHelper::assertTrue(is217Exist);
    AssertHelper::assertFalse(is317Exist);
}

CppUnit::Test* GridContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("GridContainerTest");

    suite->addTest(new CppUnit::TestCaller("findXPositiveNeighbor", &GridContainerTest::findXPositiveNeighbor));
    suite->addTest(new CppUnit::TestCaller("findXNegativeNeighbor", &GridContainerTest::findXNegativeNeighbor));
    suite->addTest(new CppUnit::TestCaller("findYPositiveAllDirectNeighbors", &GridContainerTest::findYPositiveAllDirectNeighbors));
    suite->addTest(new CppUnit::TestCaller("findYNegativeAllDirectNeighbors", &GridContainerTest::findYNegativeAllDirectNeighbors));

    suite->addTest(new CppUnit::TestCaller("removeNeighbor", &GridContainerTest::removeNeighbor));

    suite->addTest(new CppUnit::TestCaller("checkItemExist", &GridContainerTest::checkItemExist));

    return suite;
}
