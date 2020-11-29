#ifndef URCHINENGINE_NOBODYSHAPEWIDGET_H
#define URCHINENGINE_NOBODYSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinPhysicsEngine.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin {

    class NoBodyShapeWidget : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit NoBodyShapeWidget(const SceneObject*);
            ~NoBodyShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
            std::shared_ptr<const CollisionShape3D> createBodyShape() const override;
    };

}

#endif
