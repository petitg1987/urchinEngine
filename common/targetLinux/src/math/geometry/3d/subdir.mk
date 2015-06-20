################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/geometry/3d/IndexedTriangle3D.cpp \
../src/math/geometry/3d/Line3D.cpp \
../src/math/geometry/3d/Plane.cpp 

OBJS += \
./src/math/geometry/3d/IndexedTriangle3D.o \
./src/math/geometry/3d/Line3D.o \
./src/math/geometry/3d/Plane.o 

CPP_DEPS += \
./src/math/geometry/3d/IndexedTriangle3D.d \
./src/math/geometry/3d/Line3D.d \
./src/math/geometry/3d/Plane.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/geometry/3d/%.o: ../src/math/geometry/3d/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


