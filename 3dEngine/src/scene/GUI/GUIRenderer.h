#ifndef URCHINENGINE_GUIRENDERER_H
#define URCHINENGINE_GUIRENDERER_H

#include <set>
#include "UrchinCommon.h"

#include "scene/Renderer.h"
#include "scene/GUI/widget/Widget.h"

namespace urchin {

    class GUIRenderer : public Renderer, public Observer {
        public:
            GUIRenderer();
            ~GUIRenderer() override;

            void onResize(unsigned int, unsigned  int) override;
            void notify(Observable *, int) override;
            void setupSkin(const std::string &);

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(unsigned int) override;
            bool onMouseMove(int, int) override;
            void onDisable() override;

            void addWidget(Widget *);
            void removeWidget(Widget *);

            void display(float) override;

        private:
            std::vector<Widget *> widgets;

            Matrix3<float> mProjection, mModelView;
            unsigned int GUIShader;
            int mProjectionLoc, translateDistanceLoc, diffuseTexSamplerLoc;
    };

}

#endif
