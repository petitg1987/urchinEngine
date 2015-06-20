################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/body/AbstractBody.cpp \
../src/body/BodyManager.cpp \
../src/body/InertiaCalculation.cpp \
../src/body/RigidBody.cpp 

OBJS += \
./src/body/AbstractBody.o \
./src/body/BodyManager.o \
./src/body/InertiaCalculation.o \
./src/body/RigidBody.o 

CPP_DEPS += \
./src/body/AbstractBody.d \
./src/body/BodyManager.d \
./src/body/InertiaCalculation.d \
./src/body/RigidBody.d 


# Each subdirectory must supply rules for building sources it contributes
src/body/%.o: ../src/body/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


