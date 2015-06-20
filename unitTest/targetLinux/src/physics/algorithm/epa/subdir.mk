################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/physics/algorithm/epa/EPABoxTest.cpp \
../src/physics/algorithm/epa/EPAConvexHullTest.cpp \
../src/physics/algorithm/epa/EPAConvexObjectTest.cpp \
../src/physics/algorithm/epa/EPASphereTest.cpp \
../src/physics/algorithm/epa/EPATestHelper.cpp 

OBJS += \
./src/physics/algorithm/epa/EPABoxTest.o \
./src/physics/algorithm/epa/EPAConvexHullTest.o \
./src/physics/algorithm/epa/EPAConvexObjectTest.o \
./src/physics/algorithm/epa/EPASphereTest.o \
./src/physics/algorithm/epa/EPATestHelper.o 

CPP_DEPS += \
./src/physics/algorithm/epa/EPABoxTest.d \
./src/physics/algorithm/epa/EPAConvexHullTest.d \
./src/physics/algorithm/epa/EPAConvexObjectTest.d \
./src/physics/algorithm/epa/EPASphereTest.d \
./src/physics/algorithm/epa/EPATestHelper.d 


# Each subdirectory must supply rules for building sources it contributes
src/physics/algorithm/epa/%.o: ../src/physics/algorithm/epa/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/unitTest/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


