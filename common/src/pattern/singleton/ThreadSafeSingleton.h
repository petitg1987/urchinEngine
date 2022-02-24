#pragma once

#include <typeinfo>
#include <iostream>
#include <atomic>
#include <mutex>

#include <pattern/singleton/SingletonContainer.h>
#include <pattern/singleton/SingletonInterface.h>

namespace urchin {

    /**
    * Base class for thread-safe singleton
    */
    template<class T> class ThreadSafeSingleton : public SingletonInterface {
        public:
            static T& instance();
            ThreadSafeSingleton(const ThreadSafeSingleton&) = delete;
            ThreadSafeSingleton& operator=(const ThreadSafeSingleton&) = delete;
            ~ThreadSafeSingleton() override;

        protected:
            ThreadSafeSingleton() = default;

        private:
            static std::atomic<T*> objectT;
            static std::mutex mutexInstanceCreation;
    };

    #include "ThreadSafeSingleton.inl"

}
