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
    auto item1 = std::make_shared<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3<int>(4, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3<int>(5, 1, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3<int>(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighbor117 = gridContainer.findNeighbor(Point3<int>(1, 1, 7), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);
    auto neighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::POSITIVE);

    AssertHelper::assertTrue(neighbor117 == item1);
    AssertHelper::assertTrue(neighborItem1 == item3);
    AssertHelper::assertTrue(neighborItem2 == nullptr);
    AssertHelper::assertTrue(neighborItem3 == item4);
    AssertHelper::assertTrue(neighborItem4 == nullptr);
}

void GridContainerTest::findXNegativeNeighbor() {
    auto item1 = std::make_shared<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3<int>(4, 2, 7));
    auto item3 = std::make_shared<MyGridItem>(Point3<int>(5, 1, 7));
    auto item4 = std::make_shared<MyGridItem>(Point3<int>(8, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);
    gridContainer.addItem(item3);
    gridContainer.addItem(item4);

    auto neighbor317 = gridContainer.findNeighbor(Point3<int>(3, 1, 7), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem2 = gridContainer.findNeighbor(item2->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem3 = gridContainer.findNeighbor(item3->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);
    auto neighborItem4 = gridContainer.findNeighbor(item4->getGridPosition(), GridContainer<MyGridItem>::X, GridContainer<MyGridItem>::NEGATIVE);

    AssertHelper::assertTrue(neighbor317 == item1);
    AssertHelper::assertTrue(neighborItem1 == nullptr);
    AssertHelper::assertTrue(neighborItem2 == nullptr);
    AssertHelper::assertTrue(neighborItem3 == item1);
    AssertHelper::assertTrue(neighborItem4 == item3);
}

void GridContainerTest::removeNeighbor() {
    auto item1 = std::make_shared<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_shared<MyGridItem>(Point3<int>(2, 2, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);
    gridContainer.addItem(item2);

    auto neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertTrue(neighborItem1 == item2);
    gridContainer.removeItem(item2->getGridPosition());

    neighborItem1 = gridContainer.findNeighbor(item1->getGridPosition(), GridContainer<MyGridItem>::Y, GridContainer<MyGridItem>::POSITIVE);
    AssertHelper::assertTrue(neighborItem1 == nullptr);
}

void GridContainerTest::checkItemExist() {
    auto item1 = std::make_shared<MyGridItem>(Point3<int>(2, 1, 7));
    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1);

    bool is217Exist = gridContainer.isItemExist(Point3<int>(2, 1, 7));
    bool is317Exist = gridContainer.isItemExist(Point3<int>(3, 1, 7));

    AssertHelper::assertTrue(is217Exist);
    AssertHelper::assertTrue(!is317Exist);
}

CppUnit::Test* GridContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("GridContainerTest");

    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("findXPositiveNeighbor", &GridContainerTest::findXPositiveNeighbor));
    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("findXNegativeNeighbor", &GridContainerTest::findXNegativeNeighbor));

    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("removeNeighbor", &GridContainerTest::removeNeighbor));

    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("checkItemExist", &GridContainerTest::checkItemExist));

    return suite;
}
