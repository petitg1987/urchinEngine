################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/geometry/3d/shape/BoxShape.cpp \
../src/math/geometry/3d/shape/CapsuleShape.cpp \
../src/math/geometry/3d/shape/CylinderShape.cpp \
../src/math/geometry/3d/shape/Shape3D.cpp \
../src/math/geometry/3d/shape/SphereShape.cpp \
../src/math/geometry/3d/shape/Triangle3DShape.cpp 

OBJS += \
./src/math/geometry/3d/shape/BoxShape.o \
./src/math/geometry/3d/shape/CapsuleShape.o \
./src/math/geometry/3d/shape/CylinderShape.o \
./src/math/geometry/3d/shape/Shape3D.o \
./src/math/geometry/3d/shape/SphereShape.o \
./src/math/geometry/3d/shape/Triangle3DShape.o 

CPP_DEPS += \
./src/math/geometry/3d/shape/BoxShape.d \
./src/math/geometry/3d/shape/CapsuleShape.d \
./src/math/geometry/3d/shape/CylinderShape.d \
./src/math/geometry/3d/shape/Shape3D.d \
./src/math/geometry/3d/shape/SphereShape.d \
./src/math/geometry/3d/shape/Triangle3DShape.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/geometry/3d/shape/%.o: ../src/math/geometry/3d/shape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


