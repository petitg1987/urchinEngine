################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/algorithm/MathAlgorithm.cpp 

OBJS += \
./src/math/algorithm/MathAlgorithm.o 

CPP_DEPS += \
./src/math/algorithm/MathAlgorithm.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/algorithm/%.o: ../src/math/algorithm/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


