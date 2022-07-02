#pragma once

#include <string>
#include <vector>
#include <memory>

#include <pattern/singleton/SingletonInterface.h>

namespace urchin {

    struct SingletonData {
        std::string name;
        std::unique_ptr<SingletonInterface> ptr;
    };

    class SingletonContainer {
        public:
            static void registerSingleton(const std::string&, std::unique_ptr<SingletonInterface>);

            static void destroyAllSingletons();

        private:
            SingletonContainer() = default;
            ~SingletonContainer() = default;

            static std::vector<SingletonData> singletonsVector;
    };

}
