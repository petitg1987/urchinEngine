################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/algebra/point/Point2.cpp \
../src/math/algebra/point/Point3.cpp \
../src/math/algebra/point/Point4.cpp 

OBJS += \
./src/math/algebra/point/Point2.o \
./src/math/algebra/point/Point3.o \
./src/math/algebra/point/Point4.o 

CPP_DEPS += \
./src/math/algebra/point/Point2.d \
./src/math/algebra/point/Point3.d \
./src/math/algebra/point/Point4.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/algebra/point/%.o: ../src/math/algebra/point/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


