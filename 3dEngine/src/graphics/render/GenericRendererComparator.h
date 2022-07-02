#pragma once

namespace urchin {

    class GenericRenderer;

    struct GenericRendererComparator {
        bool operator()(const GenericRenderer*, const GenericRenderer*) const;
    };

}