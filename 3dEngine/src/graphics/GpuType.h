#pragma once

namespace urchin {

    enum class GpuType {
        DEDICATED, //Stand-alone graphic card with its own VRAM
        INTEGRATED, //GPU embedded in the CPU and using shared system memory
        VIRTUAL, //GPU exposed by a virtual machine
        CPU, //Software rendering
        OTHER
    };

}
