################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/object/CollisionBoxObject.cpp \
../src/object/CollisionCapsuleObject.cpp \
../src/object/CollisionConvexHullObject.cpp \
../src/object/CollisionConvexObject3D.cpp \
../src/object/CollisionCylinderObject.cpp \
../src/object/CollisionSphereObject.cpp 

OBJS += \
./src/object/CollisionBoxObject.o \
./src/object/CollisionCapsuleObject.o \
./src/object/CollisionConvexHullObject.o \
./src/object/CollisionConvexObject3D.o \
./src/object/CollisionCylinderObject.o \
./src/object/CollisionSphereObject.o 

CPP_DEPS += \
./src/object/CollisionBoxObject.d \
./src/object/CollisionCapsuleObject.d \
./src/object/CollisionConvexHullObject.d \
./src/object/CollisionConvexObject3D.d \
./src/object/CollisionCylinderObject.d \
./src/object/CollisionSphereObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/object/%.o: ../src/object/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


