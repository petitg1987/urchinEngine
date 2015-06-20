################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/physics/algorithm/broadphase/BodyTestHelper.cpp \
../src/physics/algorithm/broadphase/SweepAndPruneTest.cpp 

OBJS += \
./src/physics/algorithm/broadphase/BodyTestHelper.o \
./src/physics/algorithm/broadphase/SweepAndPruneTest.o 

CPP_DEPS += \
./src/physics/algorithm/broadphase/BodyTestHelper.d \
./src/physics/algorithm/broadphase/SweepAndPruneTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/physics/algorithm/broadphase/%.o: ../src/physics/algorithm/broadphase/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/unitTest/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


