#ifndef URCHINENGINE_GRAPHICSERVICE_H
#define URCHINENGINE_GRAPHICSERVICE_H

namespace urchin {

    class GraphicService {
        public:
            GraphicService();

            void initializeGraphic();
            void logErrors();

        private:
            bool errorDetected;
    };

}

#endif
