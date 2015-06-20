#ifndef ENGINE_MAPEDITOR_OBJECTTABLEVIEW_H
#define ENGINE_MAPEDITOR_OBJECTTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneObject *)

namespace urchin
{

	class ObjectTableView : public QTableView, public Observable
	{
		Q_OBJECT

		public:
			ObjectTableView(QWidget *parent = 0);

			enum NotificationType
			{
				SELECTION_CHANGED
			};

			bool hasSceneObjectSelected() const;
			const SceneObject *getSelectedSceneObject() const;

			void addObject(const SceneObject *);
			bool removeSelectedObject();
			void removeAllObjects();

		private:
			QStandardItemModel *objectsListModel;

			void selectionChanged(const QItemSelection &, const QItemSelection &);
	};

}

#endif
