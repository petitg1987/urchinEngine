################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/shape/CollisionBoxShape.cpp \
../src/shape/CollisionCapsuleShape.cpp \
../src/shape/CollisionCompoundShape.cpp \
../src/shape/CollisionConvexHullShape.cpp \
../src/shape/CollisionCylinderShape.cpp \
../src/shape/CollisionShape3D.cpp \
../src/shape/CollisionSphereShape.cpp 

OBJS += \
./src/shape/CollisionBoxShape.o \
./src/shape/CollisionCapsuleShape.o \
./src/shape/CollisionCompoundShape.o \
./src/shape/CollisionConvexHullShape.o \
./src/shape/CollisionCylinderShape.o \
./src/shape/CollisionShape3D.o \
./src/shape/CollisionSphereShape.o 

CPP_DEPS += \
./src/shape/CollisionBoxShape.d \
./src/shape/CollisionCapsuleShape.d \
./src/shape/CollisionCompoundShape.d \
./src/shape/CollisionConvexHullShape.d \
./src/shape/CollisionCylinderShape.d \
./src/shape/CollisionShape3D.d \
./src/shape/CollisionSphereShape.d 


# Each subdirectory must supply rules for building sources it contributes
src/shape/%.o: ../src/shape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


