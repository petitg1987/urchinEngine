#pragma once

#include <map>
#include <string>
#include <UrchinCommon.h>

#include <resources/Resource.h>

namespace urchin {

    class ResourceContainer final : public ThreadSafeSingleton<ResourceContainer> {
        public:
            friend class ThreadSafeSingleton;

            ~ResourceContainer() override;

            template<class T> std::shared_ptr<T> getResource(std::string_view) const;
            void addResource(const std::shared_ptr<Resource>&);
            void cleanResources();

        private:
            ResourceContainer();

            void cleanResources(bool);

            mutable std::mutex mutex;
            std::map<std::string, std::shared_ptr<Resource>, std::less<>> resources;
    };

    #include "ResourceContainer.inl"

}
