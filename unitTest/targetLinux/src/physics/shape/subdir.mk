################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/physics/shape/ShapeToAABBoxTest.cpp \
../src/physics/shape/ShapeToConvexObjectTest.cpp \
../src/physics/shape/ShrinkConvexHullTest.cpp 

OBJS += \
./src/physics/shape/ShapeToAABBoxTest.o \
./src/physics/shape/ShapeToConvexObjectTest.o \
./src/physics/shape/ShrinkConvexHullTest.o 

CPP_DEPS += \
./src/physics/shape/ShapeToAABBoxTest.d \
./src/physics/shape/ShapeToConvexObjectTest.d \
./src/physics/shape/ShrinkConvexHullTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/physics/shape/%.o: ../src/physics/shape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/unitTest/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


