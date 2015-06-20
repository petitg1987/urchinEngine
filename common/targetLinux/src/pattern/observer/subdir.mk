################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pattern/observer/Observable.cpp \
../src/pattern/observer/Observer.cpp 

OBJS += \
./src/pattern/observer/Observable.o \
./src/pattern/observer/Observer.o 

CPP_DEPS += \
./src/pattern/observer/Observable.d \
./src/pattern/observer/Observer.d 


# Each subdirectory must supply rules for building sources it contributes
src/pattern/observer/%.o: ../src/pattern/observer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


