#pragma once

#include <typeinfo>
#include <iostream>
#include <atomic>
#include <mutex>

#include <pattern/singleton/SingletonManager.h>
#include <pattern/singleton/SingletonInterface.h>

namespace urchin {

    /**
    * Base class for thread-safe singleton
    */
    template<class T> class ThreadSafeSingleton : public SingletonInterface {
        public:
            static T* instance();
            ~ThreadSafeSingleton() override;

        protected:
            ThreadSafeSingleton() = default;

        private:
            static std::atomic<T*> objectT;
            static std::mutex mutexInstanceCreation;
    };

    #include "ThreadSafeSingleton.inl"

}
