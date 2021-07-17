#pragma once

#include <string>
#include <map>
#include <memory>

#include <pattern/singleton/SingletonInterface.h>

#ifdef DWIN
    #ifdef __cplusplus
        extern "C" {
    #endif
    #ifdef DLIB
        #define DLL_EXPORT __declspec(dllexport)
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
#else
    #define DLL_EXPORT
#endif

namespace urchin {

    /**
     * Map with the singletons
     */
    class DLL_EXPORT SingletonContainer {
        public:
            static void registerSingleton(const std::string&, std::unique_ptr<SingletonInterface>);

            static void destroyAllSingletons();

        private:
            SingletonContainer() = default;
            ~SingletonContainer() = default;

            static std::map<std::string, std::unique_ptr<SingletonInterface>> singletons;
    };

}

#ifdef DWIN
    #ifdef __cplusplus
        }
    #endif
#endif
