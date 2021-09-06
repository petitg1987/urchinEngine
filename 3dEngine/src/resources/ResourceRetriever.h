#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <UrchinCommon.h>

#include <resources/ResourceContainer.h>
#include <resources/model/ConstMeshes.h>
#include <resources/model/ConstAnimation.h>
#include <resources/image/Image.h>
#include <resources/material/Material.h>
#include <resources/font/Font.h>
#include <loader/Loader.h>

namespace urchin {

    /**
     * Find the appropriate loader according to the extension of the file and load the resource
     */
    class ResourceRetriever : public ThreadSafeSingleton<ResourceRetriever> {
        public:
            friend class ThreadSafeSingleton<ResourceRetriever>;

            template<class T> std::shared_ptr<T> getResource(const std::string&, const std::map<std::string, std::string>& = {});

        private:
            ResourceRetriever();

            std::map<std::string, std::unique_ptr<LoaderInterface>> loadersRegistry;
    };

    #include "ResourceRetriever.inl"

}
