#include <resources/common/TagsReaderWriter.h>

namespace urchin {

    std::vector<std::string> TagsReaderWriter::loadTags(const UdaChunk* tagsChunk, const UdaParser&) {
        std::string tagsValues = tagsChunk->getStringValue();
        return StringUtil::split(tagsValues, TAGS_SEPARATOR);
    }

    void TagsReaderWriter::writeTags(UdaChunk& tagsChunk, const std::vector<std::string>& tags, UdaWriter&) {
        std::string tagsValues = StringUtil::merge(tags, TAGS_SEPARATOR);
        tagsChunk.setStringValue(tagsValues);
    }

}
