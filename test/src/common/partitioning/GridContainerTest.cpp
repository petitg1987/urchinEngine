#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/partitioning/GridContainerTest.h>
#include <AssertHelper.h>
using namespace urchin;

MyGridItem::MyGridItem(const urchin::Point3<int>& position) :
        position(position) {
}

const urchin::Point3<int>& MyGridItem::getPosition() const {
    return position;
}

void GridContainerTest::findXNeighbor() {
    auto item1 = std::make_unique<MyGridItem>(Point3<int>(2, 1, 7));
    auto item2 = std::make_unique<MyGridItem>(Point3<int>(4, 2, 7));
    auto item3 = std::make_unique<MyGridItem>(Point3<int>(5, 1, 7));
    auto item4 = std::make_unique<MyGridItem>(Point3<int>(8, 1, 7));

    GridContainer<MyGridItem> gridContainer;
    gridContainer.addItem(item1.get());
    gridContainer.addItem(item2.get());
    gridContainer.addItem(item3.get());
    gridContainer.addItem(item4.get());


}

CppUnit::Test* GridContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("GridContainerTest");

    suite->addTest(new CppUnit::TestCaller<GridContainerTest>("findXNeighbor", &GridContainerTest::findXNeighbor));

    return suite;
}
