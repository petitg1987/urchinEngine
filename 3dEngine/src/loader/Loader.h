#pragma once

#include <string>
#include <memory>

namespace urchin {

    class LoaderInterface {
        public:
            LoaderInterface() = default;
            virtual ~LoaderInterface() = default;
    };

    template<class T> class Loader : public LoaderInterface {
        public:
            ~Loader() override = default;

            virtual std::shared_ptr<T> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) = 0;
    };

}
