#include "map/save/common/TagsReaderWriter.h"

namespace urchin {

    std::vector<std::string> TagsReaderWriter::load(const UdaChunk* tagsChunk, const UdaParser&) {
        const std::string& tagsValues = tagsChunk->getStringValue();
        return StringUtil::split(tagsValues, TAGS_SEPARATOR);
    }

    void TagsReaderWriter::write(UdaChunk& tagsChunk, const std::vector<std::string>& tags) {
        std::string tagsValues = StringUtil::join(tags, TAGS_SEPARATOR);
        tagsChunk.setStringValue(tagsValues);
    }

}
