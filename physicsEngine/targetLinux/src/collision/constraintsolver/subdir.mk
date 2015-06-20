################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/constraintsolver/ConstraintSolverManager.cpp \
../src/collision/constraintsolver/ConstraintSolving.cpp 

OBJS += \
./src/collision/constraintsolver/ConstraintSolverManager.o \
./src/collision/constraintsolver/ConstraintSolving.o 

CPP_DEPS += \
./src/collision/constraintsolver/ConstraintSolverManager.d \
./src/collision/constraintsolver/ConstraintSolving.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/constraintsolver/%.o: ../src/collision/constraintsolver/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


