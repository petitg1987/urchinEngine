#include <iomanip>
#include <sstream>
#include <vector>

#include <util/StringUtil.h>
#include <util/TypeConverter.h>
#include <logger/Logger.h>
#include <system/monitor/MonitorResolution.h>

namespace urchin {

    //static
    const char MonitorResolution::ID_DELIMITER = '_';

    MonitorResolution::MonitorResolution(unsigned int width, unsigned int height, unsigned int frequency) :
            width(width),
            height(height),
            frequency(frequency) {

    }

    MonitorResolution MonitorResolution::buildFromId(const std::string& id) {
        std::vector<std::string> resolutionParts;
        StringUtil::split(id, ID_DELIMITER, resolutionParts);
        if (resolutionParts.size() != 3) {
            Logger::instance()->logError("Wrongly structured monitor resolution id: " + id);
            return MonitorResolution(1920, 1080, 60);
        }
        return MonitorResolution(TypeConverter::toUnsignedInt(resolutionParts[0]),TypeConverter::toUnsignedInt(resolutionParts[1]),TypeConverter::toUnsignedInt(resolutionParts[2]));
    }

    std::string MonitorResolution::getId() const {
        return std::to_string(width) + ID_DELIMITER + std::to_string(height) + ID_DELIMITER + std::to_string(frequency);
    }

    unsigned int MonitorResolution::getWidth() const {
        return width;
    }

    unsigned int MonitorResolution::getHeight() const {
        return height;
    }

    unsigned int MonitorResolution::getFrequency() const {
        return frequency;
    }

    std::string MonitorResolution::userDisplay(bool displayFrequency) const {
        std::string strResolution = std::to_string(width) + "x" + std::to_string(height) + " (" + retrieveAspectRatio() + ")";
        if(displayFrequency) {
            strResolution += " @ " + std::to_string(frequency) + "Hz";
        }
        return strResolution;
    }

    std::string MonitorResolution::retrieveAspectRatio() const {
        constexpr float EPSILON = 0.01f;
        constexpr float RATIO_16_9 = 16.0f / 9.0f;
        constexpr float RATIO_4_3 = 4.0f / 3.0f;
        constexpr float RATIO_16_10 = 16.0f / 10.0f;
        constexpr float RATIO_32_9 = 32.0f / 9.0f;
        constexpr float RATIO_5_4 = 5.0f / 4.0f;
        constexpr float RATIO_3_2 = 3.0f / 2.0f;
        constexpr float RATIO_3_1 = 3.0f / 1.0f;
        constexpr float RATIO_1_1 = 1.0f / 1.0f;

        float ratio = (float)width / (float)height;

        if (ratio > RATIO_16_9 - EPSILON && ratio < RATIO_16_9 + EPSILON) {
            return "16:9";
        } else if (ratio > RATIO_4_3 - EPSILON && ratio < RATIO_4_3 + EPSILON) {
            return "4:3";
        } else if (ratio > RATIO_16_10 - EPSILON && ratio < RATIO_16_10 + EPSILON) {
            return "16:10";
        } else if (ratio > RATIO_32_9 - EPSILON && ratio < RATIO_32_9 + EPSILON) {
            return "32:9";
        } else if (ratio > RATIO_5_4 - EPSILON && ratio < RATIO_5_4 + EPSILON) {
            return "5:4";
        } else if (ratio > RATIO_3_2 - EPSILON && ratio < RATIO_3_2 + EPSILON) {
            return "3:2";
        } else if (ratio > RATIO_3_1 - EPSILON && ratio < RATIO_3_1 + EPSILON) {
            return "3:1";
        } else if (ratio > RATIO_1_1 - EPSILON && ratio < RATIO_1_1 + EPSILON) {
            return "1:1";
        }

        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << ratio;
        return stream.str();
    }

}
