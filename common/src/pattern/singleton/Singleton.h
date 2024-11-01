#pragma once

#include <typeinfo>

#include <pattern/singleton/SingletonContainer.h>
#include <pattern/singleton/SingletonInterface.h>

namespace urchin {

    /**
    * Base class for singleton (no thread-safe)
    */
    template<class T> class Singleton : public SingletonInterface {
        public:
            static T& instance();
            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;
            ~Singleton() override;

        protected:
            Singleton() = default;

        private:
            static T* objectT;
    };

    #include "Singleton.inl"

}
