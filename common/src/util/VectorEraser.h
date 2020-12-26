#ifndef URCHINENGINE_VECTORERASER_H
#define URCHINENGINE_VECTORERASER_H

#include "pattern/singleton/Singleton.h"

#include <vector>

namespace urchin {

    class VectorEraser : public Singleton<VectorEraser> {
        public:
            friend class Singleton<VectorEraser>;

            template<class T> void erase(std::vector<T>&, std::size_t);
            template<class T> void erase(std::vector<T>&, typename std::vector<T>::iterator);

        private:
            VectorEraser() = default;
            ~VectorEraser() override = default;
    };

    #include "VectorEraser.inl"

}

#endif
