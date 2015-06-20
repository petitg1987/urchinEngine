################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/utils/shader/subdir.mk
-include src/utils/display/texture/subdir.mk
-include src/utils/display/geometry/sphere/subdir.mk
-include src/utils/display/geometry/points/subdir.mk
-include src/utils/display/geometry/obbox/subdir.mk
-include src/utils/display/geometry/frustum/subdir.mk
-include src/utils/display/geometry/cylinder/subdir.mk
-include src/utils/display/geometry/capsule/subdir.mk
-include src/utils/display/geometry/aabbox/subdir.mk
-include src/utils/display/geometry/subdir.mk
-include src/texture/subdir.mk
-include src/scene/renderer3d/skybox/subdir.mk
-include src/scene/renderer3d/shadow/display/subdir.mk
-include src/scene/renderer3d/shadow/data/subdir.mk
-include src/scene/renderer3d/shadow/subdir.mk
-include src/scene/renderer3d/octree/filter/subdir.mk
-include src/scene/renderer3d/octree/subdir.mk
-include src/scene/renderer3d/model/octreefilter/subdir.mk
-include src/scene/renderer3d/model/displayer/subdir.mk
-include src/scene/renderer3d/model/subdir.mk
-include src/scene/renderer3d/light/sun/subdir.mk
-include src/scene/renderer3d/light/omnidirectional/subdir.mk
-include src/scene/renderer3d/light/subdir.mk
-include src/scene/renderer3d/camera/subdir.mk
-include src/scene/renderer3d/antialiasing/subdir.mk
-include src/scene/renderer3d/subdir.mk
-include src/scene/GUI/window/subdir.mk
-include src/scene/GUI/textbox/subdir.mk
-include src/scene/GUI/text/subdir.mk
-include src/scene/GUI/staticbitmap/subdir.mk
-include src/scene/GUI/button/subdir.mk
-include src/scene/GUI/subdir.mk
-include src/scene/subdir.mk
-include src/resources/model/subdir.mk
-include src/resources/material/subdir.mk
-include src/resources/image/subdir.mk
-include src/resources/font/subdir.mk
-include src/resources/subdir.mk
-include src/loader/model/subdir.mk
-include src/loader/material/subdir.mk
-include src/loader/image/subdir.mk
-include src/loader/font/subdir.mk
-include src/loader/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: liburchin3dEngine.so

# Tool invocations
liburchin3dEngine.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -std=c++11 -L"/home/greg/Project/VideoGame/urchinEngine/common/targetLinux" -static-libstdc++ -shared -o "liburchin3dEngine.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(CC_DEPS)$(C++_DEPS)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) liburchin3dEngine.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
