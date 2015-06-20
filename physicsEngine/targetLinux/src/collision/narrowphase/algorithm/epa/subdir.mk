################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/narrowphase/algorithm/epa/EPAAlgorithm.cpp \
../src/collision/narrowphase/algorithm/epa/EPAResult.cpp \
../src/collision/narrowphase/algorithm/epa/EPAResultCollide.cpp \
../src/collision/narrowphase/algorithm/epa/EPAResultInvalid.cpp \
../src/collision/narrowphase/algorithm/epa/EPAResultNoCollide.cpp \
../src/collision/narrowphase/algorithm/epa/EPATriangleData.cpp 

OBJS += \
./src/collision/narrowphase/algorithm/epa/EPAAlgorithm.o \
./src/collision/narrowphase/algorithm/epa/EPAResult.o \
./src/collision/narrowphase/algorithm/epa/EPAResultCollide.o \
./src/collision/narrowphase/algorithm/epa/EPAResultInvalid.o \
./src/collision/narrowphase/algorithm/epa/EPAResultNoCollide.o \
./src/collision/narrowphase/algorithm/epa/EPATriangleData.o 

CPP_DEPS += \
./src/collision/narrowphase/algorithm/epa/EPAAlgorithm.d \
./src/collision/narrowphase/algorithm/epa/EPAResult.d \
./src/collision/narrowphase/algorithm/epa/EPAResultCollide.d \
./src/collision/narrowphase/algorithm/epa/EPAResultInvalid.d \
./src/collision/narrowphase/algorithm/epa/EPAResultNoCollide.d \
./src/collision/narrowphase/algorithm/epa/EPATriangleData.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/narrowphase/algorithm/epa/%.o: ../src/collision/narrowphase/algorithm/epa/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


