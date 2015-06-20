################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/narrowphase/CollisionObjectWrapper.cpp \
../src/collision/narrowphase/NarrowPhaseManager.cpp 

OBJS += \
./src/collision/narrowphase/CollisionObjectWrapper.o \
./src/collision/narrowphase/NarrowPhaseManager.o 

CPP_DEPS += \
./src/collision/narrowphase/CollisionObjectWrapper.d \
./src/collision/narrowphase/NarrowPhaseManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/narrowphase/%.o: ../src/collision/narrowphase/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


