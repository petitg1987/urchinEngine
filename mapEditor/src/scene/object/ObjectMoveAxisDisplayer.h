#pragma once

#include <Urchin3dEngine.h>

namespace urchin {

    class ObjectMoveAxisDisplayer {
        public:
            explicit ObjectMoveAxisDisplayer(Scene&);
            ~ObjectMoveAxisDisplayer();

            void displayAxis(const Point3<float>&, unsigned int);
            void cleanCurrentDisplay();

        private:
            GeometryModel& createAxisModel(const Point3<float>&, unsigned int, std::size_t);

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> objectMoveAxisModels;
    };

}
