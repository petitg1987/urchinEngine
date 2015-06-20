################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/algebra/Quaternion.cpp \
../src/math/algebra/Transform.cpp 

OBJS += \
./src/math/algebra/Quaternion.o \
./src/math/algebra/Transform.o 

CPP_DEPS += \
./src/math/algebra/Quaternion.d \
./src/math/algebra/Transform.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/algebra/%.o: ../src/math/algebra/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


