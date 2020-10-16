#ifndef URCHINENGINE_BODYSPHERESHAPEWIDGET_H
#define URCHINENGINE_BODYSPHERESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin {

    class BodySphereShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodySphereShapeWidget(const SceneObject *);
            ~BodySphereShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
            std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            QDoubleSpinBox *radius;
    };

}

#endif
