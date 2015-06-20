################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/physics/algorithm/gjk/GJKBoxTest.cpp \
../src/physics/algorithm/gjk/GJKConvexHullTest.cpp \
../src/physics/algorithm/gjk/GJKConvexObjectTest.cpp \
../src/physics/algorithm/gjk/GJKSphereTest.cpp \
../src/physics/algorithm/gjk/GJKTestHelper.cpp 

OBJS += \
./src/physics/algorithm/gjk/GJKBoxTest.o \
./src/physics/algorithm/gjk/GJKConvexHullTest.o \
./src/physics/algorithm/gjk/GJKConvexObjectTest.o \
./src/physics/algorithm/gjk/GJKSphereTest.o \
./src/physics/algorithm/gjk/GJKTestHelper.o 

CPP_DEPS += \
./src/physics/algorithm/gjk/GJKBoxTest.d \
./src/physics/algorithm/gjk/GJKConvexHullTest.d \
./src/physics/algorithm/gjk/GJKConvexObjectTest.d \
./src/physics/algorithm/gjk/GJKSphereTest.d \
./src/physics/algorithm/gjk/GJKTestHelper.d 


# Each subdirectory must supply rules for building sources it contributes
src/physics/algorithm/gjk/%.o: ../src/physics/algorithm/gjk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/unitTest/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


