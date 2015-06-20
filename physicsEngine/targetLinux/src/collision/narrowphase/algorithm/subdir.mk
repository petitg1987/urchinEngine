################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/narrowphase/algorithm/CollisionAlgorithm.cpp \
../src/collision/narrowphase/algorithm/CollisionAlgorithmBuilder.cpp \
../src/collision/narrowphase/algorithm/CollisionAlgorithmSelector.cpp \
../src/collision/narrowphase/algorithm/CompoundCollisionAlgorithm.cpp \
../src/collision/narrowphase/algorithm/ConvexHullConvexHullCollisionAlgorithm.cpp \
../src/collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.cpp \
../src/collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.cpp 

OBJS += \
./src/collision/narrowphase/algorithm/CollisionAlgorithm.o \
./src/collision/narrowphase/algorithm/CollisionAlgorithmBuilder.o \
./src/collision/narrowphase/algorithm/CollisionAlgorithmSelector.o \
./src/collision/narrowphase/algorithm/CompoundCollisionAlgorithm.o \
./src/collision/narrowphase/algorithm/ConvexHullConvexHullCollisionAlgorithm.o \
./src/collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.o \
./src/collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.o 

CPP_DEPS += \
./src/collision/narrowphase/algorithm/CollisionAlgorithm.d \
./src/collision/narrowphase/algorithm/CollisionAlgorithmBuilder.d \
./src/collision/narrowphase/algorithm/CollisionAlgorithmSelector.d \
./src/collision/narrowphase/algorithm/CompoundCollisionAlgorithm.d \
./src/collision/narrowphase/algorithm/ConvexHullConvexHullCollisionAlgorithm.d \
./src/collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.d \
./src/collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/narrowphase/algorithm/%.o: ../src/collision/narrowphase/algorithm/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


