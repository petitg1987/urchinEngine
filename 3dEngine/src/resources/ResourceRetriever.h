#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <typeinfo>

#include <resources/ResourceContainer.h>
#include <loader/Loader.h>

namespace urchin {

    class ResourceRetriever {
        public:
            static constexpr char SPECIAL_FILENAME_PREFIX = '#';

            static ResourceRetriever& instance();

            template<class T> std::shared_ptr<T> getResource(const std::string&, const std::map<std::string, std::string, std::less<>>& = {}, bool = false);

        private:
            ResourceRetriever();

            bool isFullPath(const std::string&) const;

            std::map<std::string, std::unique_ptr<LoaderInterface>, std::less<>> loadersRegistry;
    };

    #include "ResourceRetriever.inl"

}
