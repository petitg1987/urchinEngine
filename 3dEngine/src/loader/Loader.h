#pragma once

#include <string>
#include <stdexcept>

namespace urchin {

    class LoaderInterface {
        public:
            LoaderInterface() = default;
            virtual ~LoaderInterface() = default;
    };

    template<class T> class Loader : public LoaderInterface {
        public:
            Loader() = default;
            ~Loader() override;

            virtual T* loadFromFile(const std::string&, const std::map<std::string, std::string>&);
            virtual void saveToFile(const T*, const std::string&);
    };

    #include "Loader.inl"

}
