#pragma once

#include <map>
#include <string>
#include <UrchinCommon.h>

#include <resources/Resource.h>

namespace urchin {

    class ResourceContainer : public ThreadSafeSingleton<ResourceContainer> {
        public:
            friend class ThreadSafeSingleton<ResourceContainer>;

            ~ResourceContainer() override;

            template<class T> std::shared_ptr<T> getResource(const std::string&) const;
            void addResource(const std::shared_ptr<Resource>&);
            void cleanResources();

        private:
            ResourceContainer();

            void cleanResources(bool);

            mutable std::mutex mutex;
            std::map<std::string, std::shared_ptr<Resource>> resources;
    };

    #include "ResourceContainer.inl"

}
