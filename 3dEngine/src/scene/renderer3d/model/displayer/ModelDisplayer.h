#ifndef URCHINENGINE_MODELDISPLAYER_H
#define URCHINENGINE_MODELDISPLAYER_H

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/DisplayMode.h"

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            explicit ModelDisplayer(Model *, DisplayMode, std::shared_ptr<RenderTarget>);

            void notify(Observable*, int) override;

            void display(const MeshParameter&) const;

        private:
            Model *model;
            std::shared_ptr<RenderTarget> renderTarget;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
    };

}

#endif
