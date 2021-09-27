#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/partitioning/GridContainerTest.h>
#include <AssertHelper.h>
using namespace urchin;

MyGridItem::MyGridItem(const urchin::Point3<int>& position) :
        position(position) {
}

const urchin::Point3<int>& MyGridItem::getGridPosition() const {
    return position;
}

void GridContainerTest::findXPositiveNeighbor() {
    auto item1 = std::make_unique<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_unique<MyGridItem>(Point3<int>(4, 2, 7));
    auto item3 = std::make_unique<MyGridItem>(Point3<int>(5, 1, 7));
    auto item4 = std::make_unique<MyGridItem>(Point3<int>(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1.get());
    gridContainer.addItem(item2.get());
    gridContainer.addItem(item3.get());
    gridContainer.addItem(item4.get());

    MyGridItem* xPositiveNeighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    MyGridItem* xPositiveNeighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    MyGridItem* xPositiveNeighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    MyGridItem* xPositiveNeighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);

    AssertHelper::assertTrue(xPositiveNeighborItem1 == item3.get());
    AssertHelper::assertTrue(xPositiveNeighborItem2 == nullptr);
    AssertHelper::assertTrue(xPositiveNeighborItem3 == item4.get());
    AssertHelper::assertTrue(xPositiveNeighborItem4 == nullptr);
}

void GridContainerTest::findXNegativeNeighbor() {
    auto item1 = std::make_unique<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_unique<MyGridItem>(Point3<int>(4, 2, 7));
    auto item3 = std::make_unique<MyGridItem>(Point3<int>(5, 1, 7));
    auto item4 = std::make_unique<MyGridItem>(Point3<int>(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1.get());
    gridContainer.addItem(item2.get());
    gridContainer.addItem(item3.get());
    gridContainer.addItem(item4.get());

    MyGridItem* xNegativeNeighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    MyGridItem* xNegativeNeighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    MyGridItem* xNegativeNeighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    MyGridItem* xNegativeNeighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);

    AssertHelper::assertTrue(xNegativeNeighborItem1 == nullptr);
    AssertHelper::assertTrue(xNegativeNeighborItem2 == nullptr);
    AssertHelper::assertTrue(xNegativeNeighborItem3 == item1.get());
    AssertHelper::assertTrue(xNegativeNeighborItem4 == item3.get());
}

void GridContainerTest::removeNeighbor() {
    auto item1 = std::make_unique<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_unique<MyGridItem>(Point3<int>(2, 2, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1.get());
    gridContainer.addItem(item2.get());

    MyGridItem* yPositiveNeighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertTrue(yPositiveNeighborItem1 == item2.get());
    gridContainer.removeItem(item2.get());

    yPositiveNeighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertTrue(yPositiveNeighborItem1 == nullptr);
}

CppUnit::Test* GridContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("GridContainerTest");

    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("findXPositiveNeighbor", &GridContainerTest::findXPositiveNeighbor));
    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("findXNegativeNeighbor", &GridContainerTest::findXNegativeNeighbor));
    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("removeNeighbor", &GridContainerTest::removeNeighbor));

    return suite;
}
