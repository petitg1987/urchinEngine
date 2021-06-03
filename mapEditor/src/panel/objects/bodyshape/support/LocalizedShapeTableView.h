#ifndef URCHINENGINE_LOCALIZEDSHAPETABLEVIEW_H
#define URCHINENGINE_LOCALIZEDSHAPETABLEVIEW_H

#include <memory>
#include <vector>
#include <map>
#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::LocalizedCollisionShape*)

namespace urchin {

    class LocalizedShapeTableView : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit LocalizedShapeTableView(QWidget *parent = nullptr);

            enum NotificationType {
                OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED
            };

            bool hasLocalizedShapeSelected() const;
            std::shared_ptr<const LocalizedCollisionShape> getSelectedLocalizedShape() const;
            std::vector<std::shared_ptr<const LocalizedCollisionShape>> getLocalizedShapes() const;
            void updateSelectedLocalizedShape(const std::shared_ptr<const LocalizedCollisionShape>&);

            int addLocalizedShape(const std::shared_ptr<const LocalizedCollisionShape>&);
            bool removeSelectedLocalizedShape();
            void selectLocalizedShape(int);

        private:
            QStandardItemModel* localizedShapesTableModel;
            std::map<const urchin::LocalizedCollisionShape*, std::shared_ptr<const LocalizedCollisionShape>> localizedShapesMap;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;

            void addLocalizedShapeInMap(const std::shared_ptr<const LocalizedCollisionShape>&);
            void removeSelectedLocalizedShapeFromMap();
    };

}

#endif
