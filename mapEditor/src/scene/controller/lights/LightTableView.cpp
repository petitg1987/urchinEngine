#include <QHeaderView>

#include "LightTableView.h"

namespace urchin
{

	LightTableView::LightTableView(QWidget *parent) :
		QTableView(parent)
	{
		lightsListModel = new QStandardItemModel(0, 1, this);
		lightsListModel->setHorizontalHeaderItem(0, new QStandardItem("Light Name"));
		setModel(lightsListModel);

		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
		horizontalHeader()->resizeSection(0, 355);
		verticalHeader()->hide();

		setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	}

	void LightTableView::selectionChanged(const QItemSelection &, const QItemSelection &)
	{
		//hack to refresh selection
		horizontalHeader()->resizeSection(0, 341);
		horizontalHeader()->resizeSection(0, 340);

		notifyObservers(this, NotificationType::LIGHT_SELECTION_CHANGED);
	}

	bool LightTableView::hasSceneLightSelected() const
	{
		return this->currentIndex().row()!=-1;
	}

	const SceneLight *LightTableView::getSelectedSceneLight() const
	{
		QModelIndex selectedIndex = this->currentIndex();
		if(selectedIndex.row()!=-1)
		{
			return selectedIndex.data(Qt::UserRole + 1).value<const SceneLight *>();
		}
		return nullptr;
	}

	void LightTableView::addLight(const SceneLight *sceneLight)
	{
		auto *itemLightName = new QStandardItem(QString::fromStdString(sceneLight->getName()));
		itemLightName->setData(qVariantFromValue(sceneLight), Qt::UserRole + 1);
		itemLightName->setEditable(false);

		int nextRow = lightsListModel->rowCount();
		lightsListModel->insertRow(nextRow);
		lightsListModel->setItem(nextRow, 0, itemLightName);

		resizeRowsToContents();
	}

	bool LightTableView::removeSelectedLight()
	{
		if(hasSceneLightSelected())
		{
			lightsListModel->removeRow(this->currentIndex().row());
			resizeRowsToContents();

			return true;
		}

		return false;
	}

	void LightTableView::removeAllLights()
	{
		lightsListModel->removeRows(0, lightsListModel->rowCount());
		resizeRowsToContents();
	}

}
