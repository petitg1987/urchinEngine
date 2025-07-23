#pragma once

#include <map>
#include <string>
#include <memory>

#include "resources/Resource.h"

namespace urchin {

    class ResourceContainer {
        public:
            static ResourceContainer& instance();
            ~ResourceContainer();

            template<class T> std::shared_ptr<T> getResource(std::string_view) const;
            void addResource(const std::shared_ptr<Resource>&);
            void cleanResources();

        private:
            ResourceContainer() = default;

            void cleanResources(bool);

            mutable std::mutex mutex;
            std::map<std::string, std::shared_ptr<Resource>, std::less<>> resources;
    };

    #include "ResourceContainer.inl"

}
