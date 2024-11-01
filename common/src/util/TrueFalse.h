#pragma once

namespace urchin {

    struct True {
        explicit operator bool() const {
            return true;
        }
    };

    struct False {
        explicit operator bool() const {
            return false;
        }
    };

}