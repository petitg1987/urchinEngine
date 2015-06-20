################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/integration/IntegrateTransformManager.cpp \
../src/collision/integration/IntegrateVelocityManager.cpp 

OBJS += \
./src/collision/integration/IntegrateTransformManager.o \
./src/collision/integration/IntegrateVelocityManager.o 

CPP_DEPS += \
./src/collision/integration/IntegrateTransformManager.d \
./src/collision/integration/IntegrateVelocityManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/integration/%.o: ../src/collision/integration/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


