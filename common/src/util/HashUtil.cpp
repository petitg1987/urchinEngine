#include "util/HashUtil.h"

namespace urchin {

    uint32_t HashUtil::stableHash(std::string_view str) {
        //Fowler–Noll–Vo hash
        uint32_t hash = 2166136261u;
        for (char ch : str) {
            uint32_t c = static_cast<unsigned char>(ch);
            hash ^= c;
            hash *= 16777619u;
        }
        return hash;
    }

}