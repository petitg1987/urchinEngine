#pragma once

#include <string>

#include "resources/model/ConstAnimation.h"
#include "loader/Loader.h"

namespace urchin {

    struct BoneInfo {
        std::string name;
        int parent;
        unsigned int flags;
        int startIndex;
    };

    struct BaseFrameBone {
        Point3<float> pos;
        Quaternion<float> orient;
    };

    class LoaderUrchinAnim final : public Loader<ConstAnimation> {
        public:
            ~LoaderUrchinAnim() override = default;

            std::shared_ptr<ConstAnimation> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;
    };

}
