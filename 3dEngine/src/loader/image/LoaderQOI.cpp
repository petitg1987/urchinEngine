#include "libs/qoi/qoi.h"
#include "loader/image/LoaderQOI.h"

namespace urchin {

    std::shared_ptr<Image> LoaderQOI::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        qoi_desc desc;
        int expectedChannels = 4;
        std::vector<unsigned char> pixels = qoi_read(filename.c_str(), &desc, expectedChannels);

        if (pixels.empty()) {
            throw std::runtime_error("Unable to load image: " + filename);
        }

        return std::make_shared<Image>(desc.width, desc.height, Image::IMAGE_RGBA, std::move(pixels), desc.hasTransparency);
    }

}