#ifndef URCHINENGINE_MAPEDITOR_SOUNDTABLEVIEW_H
#define URCHINENGINE_MAPEDITOR_SOUNDTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneSound *)

namespace urchin
{

	class SoundTableView : public QTableView, public Observable
	{
		Q_OBJECT

		public:
			explicit SoundTableView(QWidget *parent = nullptr);

			enum NotificationType
			{
				SELECTION_CHANGED
			};

			bool hasSceneSoundSelected() const;
			const SceneSound *getSelectedSceneSound() const;

			void addSound(const SceneSound *);
			bool removeSelectedSound();
			void removeAllSounds();

		private:
			QStandardItemModel *soundsListModel;

			void selectionChanged(const QItemSelection &, const QItemSelection &) override;
	};

}

#endif
