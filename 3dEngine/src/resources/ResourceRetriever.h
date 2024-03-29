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

    class ResourceRetriever : public ThreadSafeSingleton<ResourceRetriever> {
        public:
            static constexpr char SPECIAL_FILENAME_PREFIX = '#';

            friend class ThreadSafeSingleton<ResourceRetriever>;

            template<class T> std::shared_ptr<T> getResource(const std::string&, const std::map<std::string, std::string, std::less<>>& = {}, bool = false);

        private:
            ResourceRetriever();

            bool isFullPath(const std::string&) const;

            std::map<std::string, std::unique_ptr<LoaderInterface>, std::less<>> loadersRegistry;
    };

    #include "ResourceRetriever.inl"

}
