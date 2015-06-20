################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/body/work/AbstractWorkBody.cpp \
../src/body/work/WorkGhostBody.cpp \
../src/body/work/WorkRigidBody.cpp 

OBJS += \
./src/body/work/AbstractWorkBody.o \
./src/body/work/WorkGhostBody.o \
./src/body/work/WorkRigidBody.o 

CPP_DEPS += \
./src/body/work/AbstractWorkBody.d \
./src/body/work/WorkGhostBody.d \
./src/body/work/WorkRigidBody.d 


# Each subdirectory must supply rules for building sources it contributes
src/body/work/%.o: ../src/body/work/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


