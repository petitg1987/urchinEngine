#pragma once

#ifdef WIN32
    #include <vulkan/vk_enum_string_helper.h>
#else
    static constexpr uint64_t VK_BUFFER_USAGE_2_EXECUTION_GRAPH_SCRATCH_BIT_AMDX = 0x02000000ULL; //required for compilation success
    #include <vulkan/generated/vk_enum_string_helper.h>
#endif
