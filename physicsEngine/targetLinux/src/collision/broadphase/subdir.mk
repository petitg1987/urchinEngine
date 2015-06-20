################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/broadphase/BodyBox.cpp \
../src/collision/broadphase/BodyEndPoint.cpp \
../src/collision/broadphase/BroadPhaseManager.cpp \
../src/collision/broadphase/PairContainer.cpp \
../src/collision/broadphase/SweepAndPrune.cpp \
../src/collision/broadphase/VectorPairContainer.cpp 

OBJS += \
./src/collision/broadphase/BodyBox.o \
./src/collision/broadphase/BodyEndPoint.o \
./src/collision/broadphase/BroadPhaseManager.o \
./src/collision/broadphase/PairContainer.o \
./src/collision/broadphase/SweepAndPrune.o \
./src/collision/broadphase/VectorPairContainer.o 

CPP_DEPS += \
./src/collision/broadphase/BodyBox.d \
./src/collision/broadphase/BodyEndPoint.d \
./src/collision/broadphase/BroadPhaseManager.d \
./src/collision/broadphase/PairContainer.d \
./src/collision/broadphase/SweepAndPrune.d \
./src/collision/broadphase/VectorPairContainer.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/broadphase/%.o: ../src/collision/broadphase/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


