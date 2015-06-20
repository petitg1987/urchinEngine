################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/visualizer/subdir.mk
-include src/utils/pool/subdir.mk
-include src/utils/math/subdir.mk
-include src/shape/util/subdir.mk
-include src/shape/subdir.mk
-include src/processable/character/subdir.mk
-include src/processable/subdir.mk
-include src/object/subdir.mk
-include src/collision/narrowphase/algorithm/gjk/subdir.mk
-include src/collision/narrowphase/algorithm/epa/subdir.mk
-include src/collision/narrowphase/algorithm/subdir.mk
-include src/collision/narrowphase/subdir.mk
-include src/collision/island/subdir.mk
-include src/collision/integration/subdir.mk
-include src/collision/constraintsolver/solvingdata/subdir.mk
-include src/collision/constraintsolver/subdir.mk
-include src/collision/broadphase/subdir.mk
-include src/collision/subdir.mk
-include src/body/work/subdir.mk
-include src/body/subdir.mk
-include src/subdir.mk
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
all: liburchinPhysicsEngine.so

# Tool invocations
liburchinPhysicsEngine.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L"/home/greg/Project/VideoGame/urchinEngine/common/targetLinux" -static-libstdc++ -shared -o "liburchinPhysicsEngine.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(CC_DEPS)$(C++_DEPS)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) liburchinPhysicsEngine.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
