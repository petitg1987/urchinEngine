#pragma once

namespace urchin {

    struct True {
        operator bool() const {
            return true;
        }
    };

    struct False {
        operator bool() const {
            return false;
        }
    };

}