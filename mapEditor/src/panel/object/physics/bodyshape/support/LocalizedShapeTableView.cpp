#include <QHeaderView>
#include <QVariant>

#include <panel/object/physics/bodyshape/support/LocalizedShapeTableView.h>
#include <panel/object/physics/bodyshape/BodyShapeWidget.h>
#include <panel/object/physics/bodyshape/BodyShapeWidgetRetriever.h>

namespace urchin {

    LocalizedShapeTableView::LocalizedShapeTableView(QWidget* parent) :
            QTableView(parent) {
        localizedShapesTableModel = new QStandardItemModel(0, 1, this);
        localizedShapesTableModel->setHorizontalHeaderItem(0, new QStandardItem("Shape Type"));

        QTableView::setModel(localizedShapesTableModel);
        horizontalHeader()->resizeSection(0, 325);

        setSelectionMode(SingleSelection);
        setSelectionBehavior(SelectRows);
    }

    void LocalizedShapeTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        notifyObservers(this, OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED);
    }

    bool LocalizedShapeTableView::hasLocalizedShapeSelected() const {
        return this->currentIndex().row() != -1;
    }

    const LocalizedCollisionShape* LocalizedShapeTableView::getSelectedLocalizedShape() const {
        if (hasLocalizedShapeSelected()) {
            QModelIndex selectedIndex = this->currentIndex();

            return selectedIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape*>();
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<const LocalizedCollisionShape>> LocalizedShapeTableView::getLocalizedShapes() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;
        for (int row = 0; row < localizedShapesTableModel->rowCount(); ++row) {
            QModelIndex shapeIndex = localizedShapesTableModel->index(row, 0);

            const auto* localizedCollisionShape = shapeIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape*>();
            localizedCollisionShapes.push_back(localizedShapesMap.at(localizedCollisionShape));
        }

        return localizedCollisionShapes;
    }

    void LocalizedShapeTableView::updateSelectedLocalizedShape(const std::shared_ptr<const LocalizedCollisionShape>& newLocalizedShape) {
        if (hasLocalizedShapeSelected()) {
            removeSelectedLocalizedShapeFromMap();

            QStandardItem* itemShape = localizedShapesTableModel->item(this->currentIndex().row(), 0);

            addLocalizedShapeInMap(newLocalizedShape);
            itemShape->setData(QVariant::fromValue(newLocalizedShape.get()), Qt::UserRole + 1);
        }
    }

    int LocalizedShapeTableView::addLocalizedShape(const std::shared_ptr<const LocalizedCollisionShape>& localizedShape) {
        auto* bodyShapeWidget = BodyShapeWidgetRetriever(nullptr).createBodyShapeWidget(localizedShape->shape->getShapeType());
        std::string shapeTypeString = bodyShapeWidget->getBodyShapeName();
        delete bodyShapeWidget;

        auto* itemShape = new QStandardItem(QString::fromStdString(shapeTypeString));
        addLocalizedShapeInMap(localizedShape);
        itemShape->setData(QVariant::fromValue(localizedShape.get()), Qt::UserRole + 1);
        itemShape->setEditable(false);

        int nextRow = localizedShapesTableModel->rowCount();
        localizedShapesTableModel->insertRow(nextRow);
        localizedShapesTableModel->setItem(nextRow, 0, itemShape);

        resizeRowsToContents();

        return nextRow; //row id (start to 0)
    }

    bool LocalizedShapeTableView::removeSelectedLocalizedShape() {
        if (hasLocalizedShapeSelected()) {
            removeSelectedLocalizedShapeFromMap();
            localizedShapesTableModel->removeRow(this->currentIndex().row());

            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void LocalizedShapeTableView::selectLocalizedShape(int rowId) const {
        QModelIndex indexSelection = localizedShapesTableModel->index(rowId, 0);
        if (indexSelection.row() != -1) {
            selectionModel()->setCurrentIndex(indexSelection, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
            selectionModel()->select(indexSelection, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
        }
    }

    void LocalizedShapeTableView::addLocalizedShapeInMap(const std::shared_ptr<const LocalizedCollisionShape>& localizedShape) {
        //allow keeping pointer alive when it's stored in a QVariant
        localizedShapesMap[localizedShape.get()] = localizedShape;
    }

    void LocalizedShapeTableView::removeSelectedLocalizedShapeFromMap() {
        if (hasLocalizedShapeSelected()) {
            QModelIndex shapeIndex = localizedShapesTableModel->index(this->currentIndex().row(), 0);
            const auto* localizedShape = shapeIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape*>();

            localizedShapesMap.erase(localizedShape);
        }
    }
}
