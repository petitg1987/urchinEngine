################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/system/FileHandler.cpp \
../src/system/FileSystem.cpp 

OBJS += \
./src/system/FileHandler.o \
./src/system/FileSystem.o 

CPP_DEPS += \
./src/system/FileHandler.d \
./src/system/FileSystem.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/%.o: ../src/system/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


