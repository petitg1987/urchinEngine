#pragma once

#include <typeinfo>
#include <iostream>
#include <thread>
#include <map>

#include <pattern/singleton/SingletonContainer.h>
#include <pattern/singleton/SingletonInterface.h>

namespace urchin {

    /**
    * Base class for singleton (no thread-safe)
    */
    template<class T> class Singleton : public SingletonInterface {
        public:
            static T& instance();
            ~Singleton() override;

        protected:
            Singleton() = default;

        private:
            static T* objectT;
    };

    #include "Singleton.inl"

}
