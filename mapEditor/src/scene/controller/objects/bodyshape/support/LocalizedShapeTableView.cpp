#include <QHeaderView>
#include <QVariant>

#include "LocalizedShapeTableView.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.h"

namespace urchin
{

	LocalizedShapeTableView::LocalizedShapeTableView(QWidget *parent) :
		QTableView(parent)
	{
		localizedShapesTableModel = new QStandardItemModel(0, 1, this);
		localizedShapesTableModel->setHorizontalHeaderItem(0, new QStandardItem("Shape Type"));

		setModel(localizedShapesTableModel);
		horizontalHeader()->resizeSection(0, 180);

		setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	}

	void LocalizedShapeTableView::selectionChanged(const QItemSelection &, const QItemSelection &)
	{
		notifyObservers(this, NotificationType::SELECTION_CHANGED);
	}

	bool LocalizedShapeTableView::hasLocalizedShapeSelected() const
	{
		return this->currentIndex().row()!=-1;
	}

	std::shared_ptr<const LocalizedCollisionShape> LocalizedShapeTableView::getSelectedLocalizedShape() const
	{
		if(hasLocalizedShapeSelected())
		{
			QModelIndex selectedIndex = this->currentIndex();

			const LocalizedCollisionShape *selectLocalizedShape = selectedIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape *>();
			return findLocalizedShapeInMap(selectLocalizedShape);
		}
		return std::shared_ptr<const LocalizedCollisionShape>(nullptr);
	}

	std::vector<std::shared_ptr<const LocalizedCollisionShape>> LocalizedShapeTableView::getLocalizedShapes() const
	{
		std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;
		for(int row=0; row< localizedShapesTableModel->rowCount(); ++row)
		{
			QModelIndex shapeIndex = localizedShapesTableModel->index(row, 0);

			const LocalizedCollisionShape *localizedCollisionShape = shapeIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape *>();
			localizedCollisionShapes.push_back(findLocalizedShapeInMap(localizedCollisionShape));
		}

		return localizedCollisionShapes;
	}

	void LocalizedShapeTableView::updateSelectedLocalizedShape(std::shared_ptr<const LocalizedCollisionShape> newLocalizedShape)
	{
		if(hasLocalizedShapeSelected())
		{
			removeSelectedLocalizedShapeFromMap();

			QStandardItem *itemShape = localizedShapesTableModel->item(this->currentIndex().row(), 0);

			addLocalizedShapeInMap(newLocalizedShape);
			itemShape->setData(qVariantFromValue(newLocalizedShape.get()), Qt::UserRole + 1);
		}
	}

	int LocalizedShapeTableView::addLocalizedShape(std::shared_ptr<const LocalizedCollisionShape> localizedShape)
	{
		BodyShapeWidget *bodyShapeWidget = BodyShapeWidgetRetriever(nullptr, nullptr).retrieveShapeWidget(localizedShape->shape->getShapeType());
		std::string shapeTypeString = bodyShapeWidget->getBodyShapeName();
		delete bodyShapeWidget;

		QStandardItem *itemShape = new QStandardItem(QString::fromStdString(shapeTypeString));
		addLocalizedShapeInMap(localizedShape);
		itemShape->setData(qVariantFromValue(localizedShape.get()), Qt::UserRole + 1);
		itemShape->setEditable(false);

		int nextRow = localizedShapesTableModel->rowCount();
		localizedShapesTableModel->insertRow(nextRow);
		localizedShapesTableModel->setItem(nextRow, 0, itemShape);

		resizeRowsToContents();

		return nextRow; //row id (start to 0)
	}

	bool LocalizedShapeTableView::removeSelectedLocalizedShape()
	{
		if(hasLocalizedShapeSelected())
		{
			removeSelectedLocalizedShapeFromMap();
			localizedShapesTableModel->removeRow(this->currentIndex().row());

			resizeRowsToContents();

			return true;
		}

		return false;
	}

	void LocalizedShapeTableView::selectLocalizedShape(int rowId)
	{
		QModelIndex modelIndexSelection = localizedShapesTableModel->index(rowId, 0);
		if(modelIndexSelection.row()!=-1)
		{
			selectionModel()->setCurrentIndex(modelIndexSelection, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
			selectionModel()->select(modelIndexSelection, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
		}
	}

	void LocalizedShapeTableView::addLocalizedShapeInMap(std::shared_ptr<const LocalizedCollisionShape> localizedShape)
	{
		//allow to keep pointer alive when it's stored in a QVariant
		localizedShapesMap[localizedShape.get()] = localizedShape;
	}

	void LocalizedShapeTableView::removeSelectedLocalizedShapeFromMap()
	{
		if(hasLocalizedShapeSelected())
		{
			QModelIndex shapeIndex = localizedShapesTableModel->index(this->currentIndex().row(), 0);
			const LocalizedCollisionShape *localizedShape = shapeIndex.data(Qt::UserRole + 1).value<const LocalizedCollisionShape *>();

			localizedShapesMap.erase(localizedShape);
		}
	}

	std::shared_ptr<const LocalizedCollisionShape> LocalizedShapeTableView::findLocalizedShapeInMap(const LocalizedCollisionShape *localizedShape) const
	{
		std::map<const urchin::LocalizedCollisionShape *, std::shared_ptr<const LocalizedCollisionShape>>::const_iterator itFind;
		itFind = localizedShapesMap.find(localizedShape);
		if(itFind==localizedShapesMap.end())
		{
			throw std::invalid_argument("Impossible to find localized shape in map.");
		}

		return itFind->second;
	}
}
