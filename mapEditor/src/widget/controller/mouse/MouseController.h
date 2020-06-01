#ifndef URCHINENGINE_MOUSECONTROLLER_H
#define URCHINENGINE_MOUSECONTROLLER_H

namespace urchin
{

    class SceneDisplayerWidget;

    class MouseController
    {
        public:
            MouseController(SceneDisplayerWidget *);

            void moveMouse(unsigned int, unsigned int);

        private:
            SceneDisplayerWidget *sceneDisplayerWidget;
    };

}

#endif
