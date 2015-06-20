################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/narrowphase/algorithm/gjk/GJKAlgorithm.cpp \
../src/collision/narrowphase/algorithm/gjk/GJKResult.cpp \
../src/collision/narrowphase/algorithm/gjk/GJKResultCollide.cpp \
../src/collision/narrowphase/algorithm/gjk/GJKResultInvalid.cpp \
../src/collision/narrowphase/algorithm/gjk/GJKResultNoCollide.cpp \
../src/collision/narrowphase/algorithm/gjk/Simplex.cpp 

OBJS += \
./src/collision/narrowphase/algorithm/gjk/GJKAlgorithm.o \
./src/collision/narrowphase/algorithm/gjk/GJKResult.o \
./src/collision/narrowphase/algorithm/gjk/GJKResultCollide.o \
./src/collision/narrowphase/algorithm/gjk/GJKResultInvalid.o \
./src/collision/narrowphase/algorithm/gjk/GJKResultNoCollide.o \
./src/collision/narrowphase/algorithm/gjk/Simplex.o 

CPP_DEPS += \
./src/collision/narrowphase/algorithm/gjk/GJKAlgorithm.d \
./src/collision/narrowphase/algorithm/gjk/GJKResult.d \
./src/collision/narrowphase/algorithm/gjk/GJKResultCollide.d \
./src/collision/narrowphase/algorithm/gjk/GJKResultInvalid.d \
./src/collision/narrowphase/algorithm/gjk/GJKResultNoCollide.d \
./src/collision/narrowphase/algorithm/gjk/Simplex.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/narrowphase/algorithm/gjk/%.o: ../src/collision/narrowphase/algorithm/gjk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


