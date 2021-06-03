#ifndef URCHINENGINE_BODYCONESHAPEWIDGET_H
#define URCHINENGINE_BODYCONESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyConeShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyConeShapeWidget(const SceneObject*);
            ~BodyConeShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const std::shared_ptr<const CollisionShape3D>&) override;
            std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            QDoubleSpinBox* radius;
            QDoubleSpinBox* height;
            QComboBox* orientation;
    };

}

#endif
