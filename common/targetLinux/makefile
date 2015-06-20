################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/tools/xml/tinyxml/subdir.mk
-include src/tools/xml/subdir.mk
-include src/tools/logger/subdir.mk
-include src/tools/subdir.mk
-include src/system/subdir.mk
-include src/pattern/singleton/subdir.mk
-include src/pattern/observer/subdir.mk
-include src/math/trigonometry/subdir.mk
-include src/math/geometry/3d/shape/subdir.mk
-include src/math/geometry/3d/object/subdir.mk
-include src/math/geometry/3d/subdir.mk
-include src/math/geometry/2d/shape/subdir.mk
-include src/math/geometry/2d/object/subdir.mk
-include src/math/geometry/2d/subdir.mk
-include src/math/algorithm/subdir.mk
-include src/math/algebra/vector/subdir.mk
-include src/math/algebra/point/subdir.mk
-include src/math/algebra/matrix/subdir.mk
-include src/math/algebra/subdir.mk
-include src/io/subdir.mk
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
all: liburchinCommon.so

# Tool invocations
liburchinCommon.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -std=c++11 -static-libstdc++ -shared -o "liburchinCommon.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(CC_DEPS)$(C++_DEPS)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) liburchinCommon.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
