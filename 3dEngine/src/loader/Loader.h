#ifndef URCHINENGINE_LOADER_H
#define URCHINENGINE_LOADER_H

#include <string>
#include <stdexcept>

namespace urchin {

    class LoaderInterface {

    };

    template<class T> class Loader : public LoaderInterface {
        public:
            virtual ~Loader();

            virtual T* loadFromFile(const std::string&, const std::map<std::string, std::string>&);
            virtual void saveToFile(const T*, const std::string&);
    };

    #include "Loader.inl"

}

#endif
