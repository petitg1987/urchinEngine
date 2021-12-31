#pragma once

namespace urchin {

    enum class DisplayMode {
        /* DEFAULT_MODE provide:
            * Texturing
            * Depth information
            * Diffuse information (output unit: 0)
            * Normal information and ambient factor (output unit: 1) */
        DEFAULT_MODE = 0,

        /* DEPTH_ONLY_MODE provide:
            * Depth information */
        DEPTH_ONLY_MODE
    };

}
