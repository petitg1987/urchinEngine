################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/CollisionWorld.cpp \
../src/collision/ManifoldContactPoint.cpp \
../src/collision/ManifoldResult.cpp \
../src/collision/OverlappingPair.cpp 

OBJS += \
./src/collision/CollisionWorld.o \
./src/collision/ManifoldContactPoint.o \
./src/collision/ManifoldResult.o \
./src/collision/OverlappingPair.o 

CPP_DEPS += \
./src/collision/CollisionWorld.d \
./src/collision/ManifoldContactPoint.d \
./src/collision/ManifoldResult.d \
./src/collision/OverlappingPair.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/%.o: ../src/collision/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


